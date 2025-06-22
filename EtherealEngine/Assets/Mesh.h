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
		~Mesh() = default;
		bool Initialize();

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

	private:		
		VertexBuffer<Vertex> m_VertexBuffer;
		IndexBuffer m_IndexBuffer;	
	};
}