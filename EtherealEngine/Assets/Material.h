#pragma once
#include "Core/EtherealIncludes.h"
#include "Core/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "Shaders.h"
#include "Renderer/DX11/Vertex.h"

namespace Ethereal
{
	class ETHEREAL_API Material : public IAsset
	{
	public:
		Material();
		~Material();
		virtual bool Load(const std::string& path) override;

		bool InitializeBuffers();

		ID3D11InputLayout* GetInputLayout();
	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;

		std::shared_ptr<Shaders> m_VertexShaderAsset;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_VertexBlob;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer; // Should be in mesh

		std::shared_ptr<Shaders> m_PixelShaderAsset;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_PixelBlob;

		
	};
}