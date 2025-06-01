#pragma once
#include <wrl.h>
#include <d3d12.h>

namespace EtherealEngine
{
	class EECommandQueue
	{
	public:
		EECommandQueue(
			Microsoft::WRL::ComPtr<ID3D12Device> device,
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT
		);
		~EECommandQueue();

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetQueue() const
		{
			return m_commandQueue;
		}
		D3D12_COMMAND_LIST_TYPE GetType() const
		{
			return m_type;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		D3D12_COMMAND_LIST_TYPE m_type;
	};
}