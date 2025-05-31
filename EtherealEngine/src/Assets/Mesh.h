#pragma once

#include <vector>
#include <DirectXMath.h>

namespace EtherealEngine
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
		// Optional: Tangent, Bitangent, Colors, BoneWeights
	};

	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		void SetVertices(const std::vector<Vertex>& vertices)
		{
			m_Vertices = vertices;
		}
		void SetIndices(const std::vector<uint32_t>& indices)
		{
			m_Indices = indices;
		}

		const std::vector<Vertex>& GetVertices() const
		{
			return m_Vertices;
		}
		const std::vector<uint32_t>& GetIndices() const
		{
			return m_Indices;
		}
		void SetMaterialIndex(uint32_t index)
		{
			m_MaterialIndex = index;
		}
		uint32_t GetMaterialIndex() const
		{
			return m_MaterialIndex;
		}

		// Future: Add material reference, bounding box, etc.

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_MaterialIndex = 0;
	};
}