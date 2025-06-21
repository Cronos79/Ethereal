#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "Shaders.h"
#include "Renderer/DX11/Vertex.h"
#include "Renderer/DX11/ConstantBuffer.h"

namespace Ethereal
{
	class ETHEREAL_API Material : public IAsset
	{
	public:
		Material() = default;
		~Material() = default;

		bool Initialize();	

		// Assessors 
		Shaders* GetVertexShader()
		{
			return m_VertexShaderAsset.get();
		}
		Shaders* GetPixelShader()
		{
			return m_PixelShaderAsset.get();
		}
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture() const
		{
			return m_MyTexture;
		}
		ConstantBuffer<CB_PS_pixelshader>& GetConstantBuffer()
		{
			return m_PSConstantBuffer;
		}
	private:
		std::shared_ptr<Shaders> m_VertexShaderAsset;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_VertexBlob;

		std::shared_ptr<Shaders> m_PixelShaderAsset;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_PixelBlob;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_MyTexture;

		ConstantBuffer<CB_PS_pixelshader> m_PSConstantBuffer;
	};
}