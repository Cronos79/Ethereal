#pragma once
#include <wrl.h>
#include <d3d12.h>

namespace EtherealEngine
{
	class EECommandAllocator
	{
	public:
		EECommandAllocator(
			Microsoft::WRL::ComPtr<ID3D12Device> device,
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT
		);
		~EECommandAllocator();

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetAllocator() const
		{
			return m_allocator;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
	};
}