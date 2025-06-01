#include "pch.h"
#include "EEFactory.h"
#include <stdexcept>
#include <vector>
#include "Core/EELoggerMacros.h"

using Microsoft::WRL::ComPtr;

namespace EtherealEngine
{
	EEFactory::EEFactory()
	{
		UINT flags = 0;
#if defined(_DEBUG)
		flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		if (FAILED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_factory))))
		{
			LOG_ERROR("Failed to create DXGI Factory");
		}
	}

	// Enumerate all adapters (GPUs)
	std::vector<ComPtr<IDXGIAdapter1>> EEFactory::EnumerateAdapters() const
	{
		std::vector<ComPtr<IDXGIAdapter1>> adapters;
		ComPtr<IDXGIAdapter1> adapter;
		for (UINT i = 0;
			m_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND;
			++i)
		{
			adapters.push_back(adapter);
		}
		return adapters;
	}

	// Pick the best adapter (discrete preferred)
	ComPtr<IDXGIAdapter1> EEFactory::GetBestAdapter() const
	{
		ComPtr<IDXGIAdapter1> bestAdapter;
		SIZE_T maxVRAM = 0;
		for (auto& adapter : EnumerateAdapters())
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			if (desc.DedicatedVideoMemory > maxVRAM)
			{
				maxVRAM = desc.DedicatedVideoMemory;
				bestAdapter = adapter;
			}
		}
		return bestAdapter;
	}
}