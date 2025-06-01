#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include <vector>

namespace EtherealEngine
{
	class EEFactory {
	public:
		EEFactory();
		Microsoft::WRL::ComPtr<IDXGIFactory6> GetFactory() const
		{
			return m_factory;
		}
		std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> EnumerateAdapters() const;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> GetBestAdapter() const;
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory6> m_factory;
	};
}