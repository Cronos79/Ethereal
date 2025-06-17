#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "Shaders.h"
#include "Renderer/DX11/Vertex.h"

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

	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	};
}