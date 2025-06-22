#include "Assets/Mesh.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"

namespace Ethereal
{

	Mesh::Mesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t materialIndex /*= 0*/)
		: m_Name(name),
		m_Vertices(vertices),
		m_Indices(indices),
		m_MaterialIndex(materialIndex)
	{
		Initialize(vertices, indices);
	}

	bool Mesh::Initialize(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		m_Vertices = vertices;
		m_Indices = indices;
		UploadToGPU();
		return true;
	}

	bool Mesh::Initialize(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices)
	{
		m_Vertices = std::move(vertices);
		m_Indices = std::move(indices);
		UploadToGPU();
		return true;
	}

	void Mesh::UploadToGPU()
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		if (!m_Vertices.empty() && !m_VertexBuffer.Get())
		{
			LOG_INFO("Uploading vertex buffer with {} vertices", m_Vertices.size());
			m_VertexBuffer.Initialize(device, m_Vertices.data(), (UINT)m_Vertices.size());
		}

		if (!m_Indices.empty() && !m_IndexBuffer.Get())
		{
			LOG_INFO("Uploading index buffer with {} indices", m_Indices.size());
			m_IndexBuffer.Initialize(device, reinterpret_cast<DWORD*>(m_Indices.data()), (UINT)m_Indices.size());
		}
	}

}