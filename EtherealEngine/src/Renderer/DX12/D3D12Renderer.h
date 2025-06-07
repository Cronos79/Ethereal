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
#pragma comment(lib, "d3dcompiler.lib")

#include <unordered_map>
#include <string>
#include <tuple>
#include "Camera.h"

struct PSOKey {
	std::string vertexShader;
	std::string pixelShader;
	// Add more fields if needed (input layout, blend state, etc.)

	bool operator==(const PSOKey& other) const
	{
		return vertexShader == other.vertexShader && pixelShader == other.pixelShader;
	}
};

namespace std
{
	template <>
	struct hash<PSOKey> {
		std::size_t operator()(const PSOKey& k) const
		{
			return hash<string>()(k.vertexShader) ^ (hash<string>()(k.pixelShader) << 1);
		}
	};
}

namespace EtherealEngine
{
	class ModelAsset; // Forward declaration of ModelAsset class
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

		void Draw(ModelAsset* model);

	protected:
		void SignalCommandQueue();
		void WaitForGPU();
		void MoveToNextFrame();

		void UpdateMVPBuffer();

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

		// Get main Camera
		Camera& GetCamera()
		{
			return m_camera;
		}

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

		ID3D12Resource* GetDepthStencilBuffer() const
		{
			return m_depthStencilBuffer.Get();
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const
		{
			return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
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

		// Depth stencil members
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		UINT m_dsvDescriptorSize = 0;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

		std::unordered_map<PSOKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_psoCache;

		// MVP constant buffer struct
		struct MVPBuffer
		{
			DirectX::XMFLOAT4X4 u_MVP;
		};

		Microsoft::WRL::ComPtr<ID3D12Resource> m_mvpBuffer;
		D3D12_GPU_VIRTUAL_ADDRESS m_mvpBufferAddress = 0;
		MVPBuffer m_mvpData;

		Camera m_camera;
	};
}