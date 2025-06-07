#include "pch.h"
#include "D3D12Renderer.h"
#include <stdexcept>
#include "Core/EELoggerMacros.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "Assets/ModelAsset.h"
#include "Assets/VertexShaderAsset.h"
#include "Core/EEContext.h"
#include "Assets/PixelShaderAsset.h"

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

		m_commandQueue = std::make_unique<EECommandQueue>(m_device->GetDevice());
		if (!m_commandQueue)
		{
			LOG_ERROR("Failed to create D3D12 command queue");
			return false;
		}

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
		srvHeapDesc.NumDescriptors = 64;
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

		// Create DSV descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_device->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));
		m_dsvDescriptorSize = m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		// Create depth stencil buffer
		D3D12_RESOURCE_DESC depthDesc = {};
		depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthDesc.Width = static_cast<UINT>(width);
		depthDesc.Height = static_cast<UINT>(height);
		depthDesc.DepthOrArraySize = 1;
		depthDesc.MipLevels = 1;
		depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

		hr = m_device->GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&depthDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&m_depthStencilBuffer)
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create depth stencil buffer");
			return false;
		}

		// Create depth stencil view
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		m_device->GetDevice()->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

		// ImGui setup
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(
			m_device->GetDevice().Get(),
			m_swapChain->GetBufferCount(),
			m_swapChain->GetFormat(),
			m_srvHeap.Get(),
			m_srvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_srvHeap->GetGPUDescriptorHandleForHeapStart()
		);

		m_camera.SetPerspective(DirectX::XM_PIDIV4, float(width) / float(height), 0.1f, 100.0f);
		m_camera.SetLookAt({ 0, 0, -5 }, { 0, 0, 0 }, { 0, 1, 0 });

		// Root Signature with 1 CBV (b0)
		CD3DX12_ROOT_PARAMETER rootParameters[1];
		rootParameters[0].InitAsConstantBufferView(0); // b0

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(
			1, rootParameters,
			0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signatureBlob, errorBlob;
		hr = D3D12SerializeRootSignature(
			&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			&signatureBlob, &errorBlob);
		if (FAILED(hr)) return false;

		hr = m_device->GetDevice()->CreateRootSignature(
			0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&m_rootSignature));
		if (FAILED(hr)) return false;

		// Create MVP constant buffer (256-byte aligned)
		D3D12_RESOURCE_DESC cbDesc = {};
		cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbDesc.Width = (sizeof(MVPBuffer) + 255) & ~255; // 256-byte aligned
		cbDesc.Height = 1;
		cbDesc.DepthOrArraySize = 1;
		cbDesc.MipLevels = 1;
		cbDesc.SampleDesc.Count = 1;
		cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		CD3DX12_HEAP_PROPERTIES cbHeapProps(D3D12_HEAP_TYPE_UPLOAD);

		hr = m_device->GetDevice()->CreateCommittedResource(
			&cbHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&cbDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_mvpBuffer)
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create MVP constant buffer");
			return false;
		}

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
		m_commandAllocator->GetAllocator()->Reset();
		m_commandList->GetList()->Reset(m_commandAllocator->GetAllocator().Get(), nullptr);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = GetCurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_commandList->GetList()->ResourceBarrier(1, &barrier);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCurrentRTV();
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDSV();
		m_commandList->GetList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

		ID3D12DescriptorHeap* heaps[] = { m_srvHeap.Get() };
		m_commandList->GetList()->SetDescriptorHeaps(_countof(heaps), heaps);

		const float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		m_commandList->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		m_commandList->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		return true;
	}

	bool D3D12Renderer::EndFrame()
	{
		ImGui::Render();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList->GetList().Get());

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = GetCurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_commandList->GetList()->ResourceBarrier(1, &barrier);

		m_commandList->GetList()->Close();
		ID3D12CommandList* cmdLists[] = { m_commandList->GetList().Get() };
		m_commandQueue->GetQueue()->ExecuteCommandLists(1, cmdLists);

		return true;
	}

	bool D3D12Renderer::PresentFrame()
	{
		HRESULT hr = m_swapChain->GetSwapChain()->Present(1, 0);
		if (FAILED(hr))
		{
			LOG_ERROR("SwapChain Present failed");
			return false;
		}
		MoveToNextFrame();
		return true;
	}

	void D3D12Renderer::Draw(ModelAsset* model)
	{
		if (!model) return;

		// Update the MVP buffer with the latest camera/view/proj
		UpdateMVPBuffer();

		auto& meshes = model->GetMeshes();
		auto& materials = model->GetMaterials();
		auto* device = m_device->GetDevice().Get();
		auto* cmdList = m_commandList->GetList().Get();
		auto* assetManager = EEContext::Get().GetAssetManager();

		int width = EEContext::Get().GetWindowWidth();
		int height = EEContext::Get().GetWindowHeight();

		D3D12_VIEWPORT viewport = { 0, 0, (float)width, (float)height, 0.0f, 1.0f };
		D3D12_RECT scissorRect = { 0, 0, width, height };
		cmdList->RSSetViewports(1, &viewport);
		cmdList->RSSetScissorRects(1, &scissorRect);

		for (auto& mesh : meshes)
		{
			if (mesh.GetVertices().empty() || mesh.GetIndices().empty())
				continue;

			// 1. Get the material for this mesh
			const auto& material = materials[mesh.GetMaterialIndex()];

			// 2. Build the PSO key (vertex shader only for now)
			PSOKey key{ material.GetVertexShader(), material.GetPixelShader() };

			// 3. Find or create the PSO
			ComPtr<ID3D12PipelineState> pso;
			auto it = m_psoCache.find(key);
			if (it != m_psoCache.end())
			{
				pso = it->second;
			}
			else
			{
				// 4. Load vertex and pixel shader bytecode from assets
				auto vs = assetManager->Get<VertexShaderAsset>(key.vertexShader);
				auto ps = assetManager->Get<PixelShaderAsset>(key.pixelShader); 
				if (!vs)
				{
					LOG_ERROR("Shader asset(s) missing: VS='{}'", key.vertexShader);
					continue;
				}
				if (!ps)
				{
					LOG_ERROR("Shader asset(s) missing: PS='{}'", key.pixelShader);
					continue;
				}

				// 5. 
				D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(EtherealEngine::Vertex, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(EtherealEngine::Vertex, normal),   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(EtherealEngine::Vertex, texcoord), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				};

				D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
				psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
				psoDesc.pRootSignature = m_rootSignature.Get();
				psoDesc.VS = vs->GetShaderBytecode();
				psoDesc.PS = ps->GetShaderBytecode();
				psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
				psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
				psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				psoDesc.SampleMask = UINT_MAX;
				psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
				psoDesc.NumRenderTargets = 1;
				psoDesc.RTVFormats[0] = m_swapChain->GetFormat();
				psoDesc.SampleDesc.Count = 1;

				HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
				if (FAILED(hr))
				{
					std::string errorMsg = "Failed to create PSO for VS='" + key.vertexShader + "'";
					LOG_HRERROR(hr, errorMsg);
					continue;
				}
				m_psoCache[key] = pso;
			}

			// 6. Set the PSO and root signature			
			cmdList->SetPipelineState(pso.Get());
			cmdList->SetGraphicsRootSignature(m_rootSignature.Get());

			// 7. Set up vertex/index buffers (as you already do)
			if (!mesh.m_vertexBuffer)
			{
				D3D12_RESOURCE_DESC vbDesc = {};
				vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				vbDesc.Width = sizeof(EtherealEngine::Vertex) * mesh.GetVertices().size();
				vbDesc.Height = 1;
				vbDesc.DepthOrArraySize = 1;
				vbDesc.MipLevels = 1;
				vbDesc.SampleDesc.Count = 1;
				vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

				CD3DX12_HEAP_PROPERTIES vbHeapProps(D3D12_HEAP_TYPE_UPLOAD);

				HRESULT hr = device->CreateCommittedResource(
					&vbHeapProps,
					D3D12_HEAP_FLAG_NONE,
					&vbDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(mesh.m_vertexBuffer.GetAddressOf())
				);
				if (FAILED(hr))
				{
					LOG_HRERROR(hr, "Failed to create vertex buffer resource");
					EEContext::Get().SetRunning(false);
					return;
				}

				void* pVertexData = nullptr;
				mesh.m_vertexBuffer->Map(0, nullptr, &pVertexData);
				memcpy(pVertexData, mesh.GetVertices().data(), vbDesc.Width);
				mesh.m_vertexBuffer->Unmap(0, nullptr);
			}

			if (!mesh.m_indexBuffer)
			{
				D3D12_RESOURCE_DESC ibDesc = {};
				ibDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				ibDesc.Width = sizeof(uint32_t) * mesh.GetIndices().size();
				ibDesc.Height = 1;
				ibDesc.DepthOrArraySize = 1;
				ibDesc.MipLevels = 1;
				ibDesc.SampleDesc.Count = 1;
				ibDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

				CD3DX12_HEAP_PROPERTIES ibHeapProps(D3D12_HEAP_TYPE_UPLOAD);

				HRESULT hr = device->CreateCommittedResource(
					&ibHeapProps,
					D3D12_HEAP_FLAG_NONE,
					&ibDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(mesh.m_indexBuffer.GetAddressOf())
				);
				if (FAILED(hr))
				{
					LOG_HRERROR(hr, "Failed to create index buffer resource");
					EEContext::Get().SetRunning(false);
					return;
				}

				void* pIndexData = nullptr;
				mesh.m_indexBuffer->Map(0, nullptr, &pIndexData);
				memcpy(pIndexData, mesh.GetIndices().data(), ibDesc.Width);
				mesh.m_indexBuffer->Unmap(0, nullptr);
			}

			D3D12_VERTEX_BUFFER_VIEW vbView = {};
			vbView.BufferLocation = mesh.m_vertexBuffer->GetGPUVirtualAddress();
			vbView.SizeInBytes = static_cast<UINT>(sizeof(EtherealEngine::Vertex) * mesh.GetVertices().size());
			vbView.StrideInBytes = sizeof(EtherealEngine::Vertex);

			D3D12_INDEX_BUFFER_VIEW ibView = {};
			ibView.BufferLocation = mesh.m_indexBuffer->GetGPUVirtualAddress();
			ibView.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * mesh.GetIndices().size());
			ibView.Format = DXGI_FORMAT_R32_UINT;

			cmdList->SetGraphicsRootConstantBufferView(0, m_mvpBuffer->GetGPUVirtualAddress());

			cmdList->IASetVertexBuffers(0, 1, &vbView);
			cmdList->IASetIndexBuffer(&ibView);
			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cmdList->DrawIndexedInstanced(static_cast<UINT>(mesh.GetIndices().size()), 1, 0, 0, 0);
		}
	}

	void D3D12Renderer::SignalCommandQueue()
	{
		m_commandQueue->GetQueue()->Signal(m_fence.Get(), m_fenceValue);
	}

	void D3D12Renderer::WaitForGPU()
	{
		SignalCommandQueue();
		if (m_fence->GetCompletedValue() < m_fenceValue)
		{
			m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
		m_fenceValue++;
	}

	void D3D12Renderer::MoveToNextFrame()
	{
		const UINT64 currentFenceValue = m_fenceValue;
		SignalCommandQueue();
		m_frameIndex = m_swapChain->GetSwapChain()->GetCurrentBackBufferIndex();
		if (m_fence->GetCompletedValue() < currentFenceValue)
		{
			m_fence->SetEventOnCompletion(currentFenceValue, m_fenceEvent);
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
		m_fenceValue++;
	}

	void D3D12Renderer::UpdateMVPBuffer()
	{
		using namespace DirectX;
		XMMATRIX model = XMMatrixIdentity();
		XMMATRIX view = m_camera.GetView();
		XMMATRIX proj = m_camera.GetProj();
		XMMATRIX mvp = XMMatrixTranspose(model * view * proj);
		XMStoreFloat4x4(&m_mvpData.u_MVP, mvp);

		/*printf("UpdateMVPBuffer: eye=(%f,%f,%f) at=(%f,%f,%f)\n",
			m_camera.GetEye().x, m_camera.GetEye().y, m_camera.GetEye().z,
			m_camera.GetAt().x, m_camera.GetAt().y, m_camera.GetAt().z);*/

		void* pData = nullptr;
		m_mvpBuffer->Map(0, nullptr, &pData);
		memcpy(pData, &m_mvpData, sizeof(MVPBuffer));
		m_mvpBuffer->Unmap(0, nullptr);
	}

}