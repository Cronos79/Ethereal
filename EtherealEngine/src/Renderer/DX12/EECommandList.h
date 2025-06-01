#pragma once
#include <wrl.h>
#include <d3d12.h>

namespace EtherealEngine
{
	class EECommandList
	{
	public:
		EECommandList(
			Microsoft::WRL::ComPtr<ID3D12Device> device,
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator,
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT
		);
		~EECommandList();

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetList() const
		{
			return m_commandList;
		}
		void Reset(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);
		void Close();

	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	};
}