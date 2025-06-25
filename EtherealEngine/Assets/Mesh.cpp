#include "Assets/Mesh.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"

namespace Ethereal
{
	bool Mesh::Initialize(const std::vector<uint8_t>& vertexData, const VertexBufferLayout& layout, const std::vector<uint32_t>& indices)
	{
		m_VertexData = vertexData;
		m_VertexLayout = layout;
		m_Indices = indices;
		UploadToGPU();
		return true;
	}

	bool Mesh::Initialize(std::vector<uint8_t>&& vertexData, VertexBufferLayout&& layout, std::vector<uint32_t>&& indices)
	{
		m_VertexData = std::move(vertexData);
		m_VertexLayout = std::move(layout);
		m_Indices = std::move(indices);
		UploadToGPU();
		return true;
	}

	void Mesh::UploadToGPU()
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());

		if (!m_VertexData.empty() && !m_VertexBuffer.Get())
		{
			//LOG_INFO("Uploading vertex buffer with {} bytes", m_VertexData.size());
			m_VertexBuffer.Initialize(device, m_VertexData.data(), static_cast<UINT>(m_VertexData.size()), m_VertexLayout.GetStride());
		}

		if (!m_Indices.empty() && !m_IndexBuffer.Get())
		{
			//LOG_INFO("Uploading index buffer with {} indices", m_Indices.size());
			m_IndexBuffer.Initialize(device, reinterpret_cast<DWORD*>(m_Indices.data()), (UINT)m_Indices.size());
		}
	}
}