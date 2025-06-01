#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "EEFactory.h"
#include "EEDevice.h"
#include <memory>
#include "EESwapChain.h"
#include "EECommandQueue.h"
#include "EECommandAllocator.h"
#include "EECommandList.h"
#include "d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace EtherealEngine
{
	class D3D12Renderer
	{
	public:
		D3D12Renderer();
		virtual ~D3D12Renderer();

		// Initialization and shutdown
		virtual bool Initialize();
		virtual void Shutdown();

		bool BeginFrame();
		bool EndFrame();
		bool PresentFrame();

	protected:
		void SignalCommandQueue();
		void WaitForGPU();
		void MoveToNextFrame();

	public:
		// Accessors
		EEFactory& GetFactory()
		{
			return *m_factory;
		}
		EEDevice& GetDevice()
		{
			return *m_device;
		}
		EECommandQueue& GetCommandQueue()
		{
			return *m_commandQueue;
		}
		EESwapChain& GetSwapChain()
		{
			return *m_swapChain;
		}
		EECommandAllocator& GetCommandAllocator()
		{
			return *m_commandAllocator;
		}
		EECommandList& GetCommandList()
		{
			return *m_commandList;
		}
		ID3D12DescriptorHeap* GetSRVHeap() const
		{
			return m_srvHeap.Get();
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle(UINT index) const
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_srvHeap->GetCPUDescriptorHandleForHeapStart(), index, m_srvDescriptorSize);
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle(UINT index) const
		{
			return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_srvHeap->GetGPUDescriptorHandleForHeapStart(), index, m_srvDescriptorSize);
		}

	public:
		ID3D12Resource* GetCurrentBackBuffer() const
		{
			return m_backBuffers[m_frameIndex].Get();
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(
				m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
				m_frameIndex, m_rtvDescriptorSize);
		}

	protected:
		std::unique_ptr<EEFactory> m_factory;
		std::unique_ptr<EEDevice> m_device;
		std::unique_ptr<EECommandQueue> m_commandQueue;
		std::unique_ptr<EESwapChain> m_swapChain;
		std::unique_ptr<EECommandAllocator> m_commandAllocator;
		std::unique_ptr<EECommandList> m_commandList;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_backBuffers;
		UINT m_rtvDescriptorSize = 0;
		UINT m_frameIndex = 0;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;
		UINT m_srvDescriptorSize = 0;

		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		UINT64 m_fenceValue = 0;
		HANDLE m_fenceEvent = nullptr;
	};
}