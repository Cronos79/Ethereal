#include "pch.h"
#include "EESwapChain.h"
#include <stdexcept>
#include "Core/EELoggerMacros.h"

namespace EtherealEngine
{
	EESwapChain::EESwapChain(
		Microsoft::WRL::ComPtr<IDXGIFactory6> factory,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
		HWND hwnd,
		UINT width,
		UINT height,
		DXGI_FORMAT format,
		UINT bufferCount)
		: m_format(format), m_bufferCount(bufferCount)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = bufferCount;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = format;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.Stereo = FALSE;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		HRESULT hr = factory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			hwnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create DXGI SwapChain for HWND: {}", hr);
		}

		// Disable Alt+Enter fullscreen
		factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

		hr = swapChain1.As(&m_swapChain);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to query IDXGISwapChain4 interface: {}", hr);
		}
	}

	EESwapChain::~EESwapChain()
	{
		m_swapChain.Reset();
	}
}