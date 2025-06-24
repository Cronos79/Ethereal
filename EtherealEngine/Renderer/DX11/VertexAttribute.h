#pragma once
#include <string>
#include <d3d11.h>

namespace Ethereal
{
	// High-level attribute role
	enum class VertexAttributeType
	{
		Position,
		Normal,
		TexCoord,
		Tangent,
		Bitangent,
		Color,
		BoneWeights,
		BoneIndices
	};

	struct VertexAttribute
	{
		VertexAttributeType type;
		DXGI_FORMAT format;
		std::string semantic;
		UINT index = 0;
		UINT offset = D3D11_APPEND_ALIGNED_ELEMENT;
	};
}