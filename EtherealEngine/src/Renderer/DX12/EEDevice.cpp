#include "EEDevice.h"
#include "pch.h"
#include <stdexcept>
#include "Core/EELoggerMacros.h"

using Microsoft::WRL::ComPtr;

namespace EtherealEngine
{
	EEDevice::EEDevice(ComPtr<IDXGIAdapter1> adapter)
	{
		HRESULT hr = D3D12CreateDevice(
			adapter.Get(),
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&m_device)
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create D3D12 device.");
		}
	}

	EEDevice::~EEDevice()
	{
		m_device.Reset();
	}
}