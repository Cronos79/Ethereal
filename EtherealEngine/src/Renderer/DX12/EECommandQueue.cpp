#include "pch.h"
#include "EECommandQueue.h"
#include <stdexcept>

namespace EtherealEngine
{
	EECommandQueue::EECommandQueue(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		D3D12_COMMAND_LIST_TYPE type
	)
		: m_type(type)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		HRESULT hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create D3D12 command queue.");
		}
	}

	EECommandQueue::~EECommandQueue()
	{
		m_commandQueue.Reset();
	}
}