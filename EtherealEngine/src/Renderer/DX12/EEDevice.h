#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace EtherealEngine
{
	class EEDevice {
	public:
		EEDevice(Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter);
		~EEDevice();

		Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const
		{
			return m_device;
		}

		// Add device-level helpers as needed

	private:
		Microsoft::WRL::ComPtr<ID3D12Device14> m_device;
	};
}