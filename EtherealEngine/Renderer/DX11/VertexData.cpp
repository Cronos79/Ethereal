#include "VertexData.h"
#include <d3d11.h>
#include "Core/Logger.h"

namespace Ethereal
{
	void VertexData::SetLayout(const VertexBufferLayout& layout)
	{
		m_Layout = layout;
		UINT totalSize = 0;

		for (const auto& desc : m_Layout.GetLayout())
		{
			UINT size = 0;
			switch (desc.Format)
			{
			case DXGI_FORMAT_R32_FLOAT: size = 4; break;
			case DXGI_FORMAT_R32G32_FLOAT: size = 8; break;
			case DXGI_FORMAT_R32G32B32_FLOAT: size = 12; break;
			case DXGI_FORMAT_R32G32B32A32_FLOAT: size = 16; break;
			default:
				LOG_WARN("Unknown format size in VertexData::SetLayout");
				break;
			}
			totalSize += size;
		}

		m_Stride = totalSize;
	}

	void VertexData::SetData(const void* data, size_t size)
	{
		m_Data.resize(size);
		std::memcpy(m_Data.data(), data, size);
	}

	bool VertexData::Upload(ID3D11Device* device)
	{
		if (m_Data.empty())
		{
			LOG_ERROR("VertexData::Upload called with empty data.");
			return false;
		}

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = static_cast<UINT>(m_Data.size());
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_Data.data();

		HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &m_VertexBuffer);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create vertex buffer in VertexData::Upload: {}", hr);
			return false;
		}

		return true;
	}
}