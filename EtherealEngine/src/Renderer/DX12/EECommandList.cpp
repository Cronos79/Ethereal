#include "pch.h"
#include "EECommandList.h"
#include <stdexcept>

namespace EtherealEngine
{
	EECommandList::EECommandList(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator,
		D3D12_COMMAND_LIST_TYPE type
	)
	{
		HRESULT hr = device->CreateCommandList(
			0, // nodeMask
			type,
			allocator.Get(),
			nullptr, // initial PSO
			IID_PPV_ARGS(&m_commandList)
		);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create D3D12 command list.");
		}
		// Command lists are created in the recording state, so close it initially
		m_commandList->Close();
	}

	EECommandList::~EECommandList()
	{
		m_commandList.Reset();
	}

	void EECommandList::Reset(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator)
	{
		HRESULT hr = m_commandList->Reset(allocator.Get(), nullptr);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to reset D3D12 command list.");
		}
	}

	void EECommandList::Close()
	{
		m_commandList->Close();
	}
}