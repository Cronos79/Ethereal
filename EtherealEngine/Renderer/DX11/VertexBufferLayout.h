#pragma once
#include <vector>
#include <d3d11.h>
#include <string>
#include "VertexAttribute.h"

namespace Ethereal
{
	class VertexBufferLayout
	{
	public:
		void AddAttribute(VertexAttributeType type, DXGI_FORMAT format, const std::string& semantic, UINT index = 0, UINT offset = D3D11_APPEND_ALIGNED_ELEMENT);
		void Build();
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& GetLayout() const;

	private:
		std::vector<VertexAttribute> m_Attributes;
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_Layout;
		std::vector<std::string> m_SemanticNames; // owns strings for SemanticName pointers
	};
}