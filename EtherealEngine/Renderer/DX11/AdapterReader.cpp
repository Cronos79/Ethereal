#include "Renderer/DX11/AdapterReader.h"
#include <wrl/client.h>
#include <dxgi.h>
#include "Core/Logger.h"

namespace Ethereal
{
	std::vector<AdapterData> AdapterReader::adapters;

	std::vector<Ethereal::AdapterData> AdapterReader::GetAdapters()
	{
		if (adapters.size() > 0)
		{
			return adapters;
		}

		Microsoft::WRL::ComPtr<IDXGIFactory1> pFactory;

		// Create the factory
		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create DXGI factory: {}", hr);
			exit(-1);
		}

		UINT adapterIndex = 0;
		Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
		while (SUCCEEDED(pFactory->EnumAdapters(adapterIndex, &pAdapter)))
		{
			// Use ComPtr directly
			adapters.emplace_back(pAdapter);
			adapterIndex++;
			pAdapter.Reset(); // Important: clear for next iteration
		}

		// (Optional) Remove duplicate check, as ComPtr handles uniqueness by pointer value

		return adapters;
	}

	AdapterData::AdapterData(Microsoft::WRL::ComPtr<IDXGIAdapter> adapter)
		: pAdapter(adapter)
	{
		if (pAdapter)
		{
			HRESULT hr = pAdapter->GetDesc(&desc);
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to get adapter description: {}", hr);
			}
		}
	}
}