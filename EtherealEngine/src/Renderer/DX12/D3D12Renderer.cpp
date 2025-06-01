#include "pch.h"
#include "D3D12Renderer.h"
#include <stdexcept>
#include "Core/EELoggerMacros.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>

using Microsoft::WRL::ComPtr;

namespace EtherealEngine
{
	D3D12Renderer::D3D12Renderer()
	{
	}

	D3D12Renderer::~D3D12Renderer()
	{
		Shutdown();
	}

	bool D3D12Renderer::Initialize()
	{
		LOG_INFO("Initializing D3D12 Renderer...");
#if defined(_DEBUG)
		// Enable the D3D12 debug layer if available
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
#endif

		m_factory = std::make_unique<EEFactory>();
		if (!m_factory)
		{
			LOG_ERROR("Failed to create D3D12 factory");
			return false;
		}
		// Get the best adapter from the factory
		auto adapter = m_factory->GetBestAdapter();
		if (!adapter)
		{
			LOG_ERROR("Failed to get a suitable adapter for D3D12");
			return false;
		}

		m_device = std::make_unique<EEDevice>(adapter);
		if (!m_device)
		{
			LOG_ERROR("Failed to create D3D12 device");
			return false;
		}

		// Create the command queue
		m_commandQueue = std::make_unique<EECommandQueue>(m_device->GetDevice());
		if (!m_commandQueue)
		{
			LOG_ERROR("Failed to create D3D12 command queue");
			return false;
		}

		// --- Swap Chain Creation ---
		HWND hwnd = EEContext::Get().GetWindowHandle();
		int width = EEContext::Get().GetWindowWidth();
		int height = EEContext::Get().GetWindowHeight();

		m_swapChain = std::make_unique<EESwapChain>(
			m_factory->GetFactory(),
			m_commandQueue->GetQueue(),
			hwnd,
			static_cast<UINT>(width),
			static_cast<UINT>(height)
		);

		if (!m_swapChain)
		{
			LOG_ERROR("Failed to create swap chain");
			return false;
		}

		m_commandAllocator = std::make_unique<EECommandAllocator>(m_device->GetDevice());
		if (!m_commandAllocator)
		{
			LOG_ERROR("Failed to create command allocator");
			return false;
		}

		m_commandList = std::make_unique<EECommandList>(m_device->GetDevice(), m_commandAllocator->GetAllocator());
		if (!m_commandList)
		{
			LOG_ERROR("Failed to create command list");
			return false;
		}

		// Create RTV descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = m_swapChain->GetBufferCount();
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_device->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
		m_rtvDescriptorSize = m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Get back buffers and create RTVs
		m_backBuffers.resize(m_swapChain->GetBufferCount());
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < m_swapChain->GetBufferCount(); ++i)
		{
			m_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]));
			m_device->GetDevice()->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
		m_frameIndex = m_swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

		// Create SRV descriptor heap (shader-visible, for ImGui and textures)
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 64; // Or higher if you plan to use more textures
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_device->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap));
		m_srvDescriptorSize = m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Create fence
		HRESULT hr = m_device->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create D3D12 fence");
			return false;
		}
		m_fenceValue = 1;
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!m_fenceEvent)
		{
			LOG_ERROR("Failed to create fence event");
			return false;
		}

		// 1. Create ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Optional

		// 2. Setup ImGui style
		ImGui::StyleColorsDark();

		// 3. Initialize ImGui Win32 and DX12 backends
		ImGui_ImplWin32_Init(hwnd); // hwnd from your swap chain creation
		ImGui_ImplDX12_Init(
			m_device->GetDevice().Get(),
			m_swapChain->GetBufferCount(),
			m_swapChain->GetFormat(),
			m_srvHeap.Get(),
			m_srvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_srvHeap->GetGPUDescriptorHandleForHeapStart()
		);

		LOG_INFO("D3D12 Renderer initialized successfully");
		return true;
	}

	void D3D12Renderer::Shutdown()
	{
		m_swapChain.reset();
		m_device.reset();
		m_factory.reset();
		if (m_fenceEvent)
		{
			CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
		}
		m_fence.Reset();

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	bool D3D12Renderer::BeginFrame()
	{
		// Reset allocator and command list
		m_commandAllocator->GetAllocator()->Reset();
		m_commandList->GetList()->Reset(m_commandAllocator->GetAllocator().Get(), nullptr);

		// Transition back buffer to render target
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = GetCurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_commandList->GetList()->ResourceBarrier(1, &barrier);

		// Set render target
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCurrentRTV();
		m_commandList->GetList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Set descriptor heaps (for ImGui, etc.)
		ID3D12DescriptorHeap* heaps[] = { m_srvHeap.Get() };
		m_commandList->GetList()->SetDescriptorHeaps(_countof(heaps), heaps);

		// Optionally clear the render target
		const float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_commandList->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// Before rendering your scene
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		return true;
	}


	bool D3D12Renderer::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList->GetList().Get());

		// Transition back buffer to present
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = GetCurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_commandList->GetList()->ResourceBarrier(1, &barrier);

		// Close and execute command list
		m_commandList->GetList()->Close();
		ID3D12CommandList* cmdLists[] = { m_commandList->GetList().Get() };
		m_commandQueue->GetQueue()->ExecuteCommandLists(1, cmdLists);

		return true;
	}

	bool D3D12Renderer::PresentFrame()
	{
		// Present
		HRESULT hr = m_swapChain->GetSwapChain()->Present(1, 0);
		if (FAILED(hr))
		{
			LOG_ERROR("SwapChain Present failed");
			return false;
		}

		// Move to next frame (fence sync and update frame index)
		MoveToNextFrame();
		return true;
	}


	void D3D12Renderer::SignalCommandQueue()
	{
		m_commandQueue->GetQueue()->Signal(m_fence.Get(), m_fenceValue);
	}


	void D3D12Renderer::WaitForGPU()
	{
		// Schedule a Signal command in the queue.
		SignalCommandQueue();

		// Wait until the fence has been processed.
		if (m_fence->GetCompletedValue() < m_fenceValue)
		{
			m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
		m_fenceValue++;
	}


	void D3D12Renderer::MoveToNextFrame()
	{
		// Signal and increment the fence value.
		const UINT64 currentFenceValue = m_fenceValue;
		SignalCommandQueue();

		m_frameIndex = m_swapChain->GetSwapChain()->GetCurrentBackBufferIndex();

		// Wait until the next frame is ready.
		if (m_fence->GetCompletedValue() < currentFenceValue)
		{
			m_fence->SetEventOnCompletion(currentFenceValue, m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
		m_fenceValue++;
	}

}