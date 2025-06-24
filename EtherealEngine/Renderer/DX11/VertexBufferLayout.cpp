#include "VertexBufferLayout.h"

namespace Ethereal
{
	void VertexBufferLayout::AddAttribute(VertexAttributeType type, DXGI_FORMAT format, const std::string& semantic, UINT index, UINT offset)
	{
		m_Attributes.push_back(VertexAttribute{ type, format, semantic, index, offset });
	}

	//void VertexBufferLayout::Build()
	//{
	//	m_Layout.clear();
	//	m_SemanticNames.clear();

	//	for (const auto& attr : m_Attributes)
	//	{
	//		D3D11_INPUT_ELEMENT_DESC desc = {};
	//		m_SemanticNames.push_back(attr.semantic); // store string
	//		desc.SemanticName = m_SemanticNames.back().c_str(); // safe: string owned in vector
	//		desc.SemanticIndex = attr.index;
	//		desc.Format = attr.format;
	//		desc.InputSlot = 0;
	//		desc.AlignedByteOffset = attr.offset;
	//		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//		desc.InstanceDataStepRate = 0;

	//		m_Layout.push_back(desc);
	//	}
	//}

	void VertexBufferLayout::Build()
	{
		m_Layout.clear();
		m_SemanticNames.clear();
		m_Stride = 0;

		for (const auto& attr : m_Attributes)
		{
			D3D11_INPUT_ELEMENT_DESC desc = {};
			m_SemanticNames.push_back(attr.semantic); // store string
			desc.SemanticName = m_SemanticNames.back().c_str(); // safe: string owned in vector
			desc.SemanticIndex = attr.index;
			desc.Format = attr.format;
			desc.InputSlot = 0;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;

			if (attr.offset == D3D11_APPEND_ALIGNED_ELEMENT)
			{
				desc.AlignedByteOffset = m_Stride;
			}
			else
			{
				desc.AlignedByteOffset = attr.offset;
			}

			// Calculate size of this element and update stride
			UINT size = 0;
			switch (desc.Format)
			{
			case DXGI_FORMAT_R32_FLOAT: size = 4; break;
			case DXGI_FORMAT_R32G32_FLOAT: size = 8; break;
			case DXGI_FORMAT_R32G32B32_FLOAT: size = 12; break;
			case DXGI_FORMAT_R32G32B32A32_FLOAT: size = 16; break;
				// Add other formats if needed
			default: size = 0; break;
			}

			if (desc.AlignedByteOffset == m_Stride)
				m_Stride += size;

			m_Layout.push_back(desc);
		}
	}

	const std::vector<D3D11_INPUT_ELEMENT_DESC>& VertexBufferLayout::GetLayout() const
	{
		return m_Layout;
	}
}