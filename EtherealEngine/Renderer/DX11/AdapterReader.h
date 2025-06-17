#pragma once
#include "Core/EtherealIncludes.h"
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

namespace Ethereal
{
	class ETHEREAL_API AdapterData
	{
	public:
		AdapterData(Microsoft::WRL::ComPtr<IDXGIAdapter> adapter);
		Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter;
		DXGI_ADAPTER_DESC desc = {};
	};

	class ETHEREAL_API AdapterReader
	{
	public:
		static std::vector<AdapterData> GetAdapters();
	private:
		static std::vector<AdapterData> adapters;
	};
}