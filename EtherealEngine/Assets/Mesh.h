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

#include "Renderer/DX11/ConstantBufferTypes.h"
#include "Renderer/DX11/ConstantBuffer.h"

namespace Ethereal
{
	class ETHEREAL_API Mesh : public IAsset
	{
	public:
		Mesh() = default;
		~Mesh() = default;
		bool Initialize(Shaders* vertexShaderAsset);
		bool InitializeBuffers();

		ID3D11InputLayout* GetInputLayout();

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

		ConstantBuffer<CB_VS_vertexshader>& GetConstantBuffer()
		{
			return m_ConstantBuffer;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
		VertexBuffer<Vertex> m_VertexBuffer;
		IndexBuffer m_IndexBuffer;	
		ConstantBuffer<CB_VS_vertexshader> m_ConstantBuffer;
	};
}