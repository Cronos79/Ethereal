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

		// Setters
		void SetDiffuseTextureName(const std::string& name);
		void SetNormalTextureName(const std::string& name);
		void SetSpecularTextureName(const std::string& name);
		void SetMetallicTextureName(const std::string& name);
		void SetRoughnessTextureName(const std::string& name);
		void SetDiffuseColor(const DirectX::XMFLOAT3& color);
		void SetVertexShaderName(const std::string& name);
		void SetPixelShaderName(const std::string& name);

		bool Initialize();	
		bool ResolveTextures();
		bool ResolveShaders();

		// Assessors 
		Shaders* GetVertexShader()
		{
			return m_VertexShaderAsset.get();
		}
		Shaders* GetPixelShader()
		{
			return m_PixelShaderAsset.get();
		}
	
		ConstantBuffer<CB_PS_pixelshader>& GetConstantBuffer()
		{
			return m_PSConstantBuffer;
		}

		const std::string& GetDiffuseTextureName() const;
		ID3D11ShaderResourceView* GetDiffuseTexture() const;
		ID3D11ShaderResourceView* GetNormalTexture() const;
		ID3D11ShaderResourceView* GetSpecularTexture() const;
		ID3D11ShaderResourceView* GetMetallicTexture() const;
		ID3D11ShaderResourceView* GetRoughnessTexture() const;
		const DirectX::XMFLOAT3& GetDiffuseColor() const;
	private:
		std::string m_DiffuseTextureName;
		std::string m_NormalTextureName;
		std::string m_SpecularTextureName;
		std::string m_MetallicTextureName;
		std::string m_RoughnessTextureName;
		DirectX::XMFLOAT3 m_DiffuseColor;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_DiffuseTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_NormalTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SpecularTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_MetallicTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_RoughnessTextureView;

		std::shared_ptr<Shaders> m_VertexShaderAsset;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_VertexBlob;
		std::string m_VertexShaderName;

		std::shared_ptr<Shaders> m_PixelShaderAsset;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_PixelBlob;
		std::string m_PixelShaderName;

		ConstantBuffer<CB_PS_pixelshader> m_PSConstantBuffer;
	};
}