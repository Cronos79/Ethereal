#include "VertexBufferLayout.h"

namespace Ethereal
{
	void VertexBufferLayout::AddAttribute(VertexAttributeType type, DXGI_FORMAT format, const std::string& semantic, UINT index, UINT offset)
	{
		m_Attributes.push_back(VertexAttribute{ type, format, semantic, index, offset });
	}

	void VertexBufferLayout::Build()
	{
		m_Layout.clear();
		m_SemanticNames.clear();

		for (const auto& attr : m_Attributes)
		{
			D3D11_INPUT_ELEMENT_DESC desc = {};
			m_SemanticNames.push_back(attr.semantic); // store string
			desc.SemanticName = m_SemanticNames.back().c_str(); // safe: string owned in vector
			desc.SemanticIndex = attr.index;
			desc.Format = attr.format;
			desc.InputSlot = 0;
			desc.AlignedByteOffset = attr.offset;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;

			m_Layout.push_back(desc);
		}
	}

	const std::vector<D3D11_INPUT_ELEMENT_DESC>& VertexBufferLayout::GetLayout() const
	{
		return m_Layout;
	}
}