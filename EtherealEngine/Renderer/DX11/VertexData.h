#pragma once
#include <vector>
#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>
#include "VertexBufferLayout.h"

namespace Ethereal
{
	class VertexData
	{
	public:
		// Set layout (must match how you fill the data)
		void SetLayout(const VertexBufferLayout& layout);

		// Add raw data
		void SetData(const void* data, size_t size);

		// Upload to GPU
		bool Upload(ID3D11Device* device);

		// Accessors for rendering
		ID3D11Buffer* GetBuffer() const
		{
			return m_VertexBuffer.Get();
		}
		const UINT* GetStridePtr() const
		{
			return &m_Stride;
		}

		const VertexBufferLayout& GetLayout() const
		{
			return m_Layout;
		}

	private:
		VertexBufferLayout m_Layout;
		std::vector<uint8_t> m_Data;

		UINT m_Stride = 0;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	};
}