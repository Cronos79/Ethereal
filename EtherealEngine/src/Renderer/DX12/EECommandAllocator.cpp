#include "pch.h"
#include "EECommandAllocator.h"
#include <stdexcept>

namespace EtherealEngine
{
	EECommandAllocator::EECommandAllocator(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type
	)
	{
		HRESULT hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_allocator));
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create D3D12 command allocator.");
		}
	}

	EECommandAllocator::~EECommandAllocator()
	{
		m_allocator.Reset();
	}
}