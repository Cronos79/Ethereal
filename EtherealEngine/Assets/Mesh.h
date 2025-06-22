#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "Shaders.h"
#include "Renderer/DX11/Vertex.h"
#include "Renderer/DX11/VertexBuffer.h"
#include "Renderer/DX11/IndexBuffer.h"

namespace Ethereal
{
	class ETHEREAL_API Mesh : public IAsset
	{
	public:
		Mesh() = default;
		Mesh(const std::string& name,
			const std::vector<Vertex>& vertices,
			const std::vector<uint32_t>& indices,
			uint32_t materialIndex = 0);
		~Mesh() = default;
		bool Initialize(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices); // copy
		bool Initialize(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices);
		void UploadToGPU();

		ID3D11Buffer* GetVertexBuffer() const
		{
			return m_VertexBuffer.Get();
		}
		const UINT* GetStridePtr() const
		{
			return m_VertexBuffer.StridePtr();
		}

		ID3D11Buffer* GetIndexBuffer() const
		{
			return m_IndexBuffer.Get();
		}

		UINT GetIndexCount() const
		{
			return m_IndexBuffer.IndexCount();
		}	

		inline std::vector<Vertex>& GetVertices()
		{
			return m_Vertices;
		}
		inline std::vector<uint32_t>& GetIndices()
		{
			return m_Indices;
		}

		void SetName(const std::string& name)
		{
			m_Name = name;
		}
		const std::string& GetName() const
		{
			return m_Name;
		}
		void SetMaterialIndex(uint32_t index)
		{
			m_MaterialIndex = index;
		}
		uint32_t GetMaterialIndex() const
		{
			return m_MaterialIndex;
		}

	private:		
		VertexBuffer<Vertex> m_VertexBuffer;
		IndexBuffer m_IndexBuffer;	
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_MaterialIndex = 0;
		std::string m_Name;
	};
}