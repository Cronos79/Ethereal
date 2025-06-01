#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

namespace EtherealEngine
{
	class EESwapChain
	{
	public:
		EESwapChain(
			Microsoft::WRL::ComPtr<IDXGIFactory6> factory,
			Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
			HWND hwnd,
			UINT width,
			UINT height,
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
			UINT bufferCount = 2
		);
		~EESwapChain();

		Microsoft::WRL::ComPtr<IDXGISwapChain3> GetSwapChain() const
		{
			return m_swapChain;
		}
		DXGI_FORMAT GetFormat() const
		{
			return m_format;
		}
		UINT GetBufferCount() const
		{
			return m_bufferCount;
		}

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		DXGI_FORMAT m_format;
		UINT m_bufferCount;
	};
}