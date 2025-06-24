#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "Shaders.h"
#include "Renderer/DX11/ConstantBuffer.h"
#include <nlohmann/json.hpp>

namespace Ethereal
{
	struct MaterialOverride
	{
		int32_t ID = -1;
		std::string vertexShader = "VertexShader";
		std::string pixelShader = "PixelShader";
		std::string diffuseTexture = "Textures/UV.png";
		nlohmann::json inputLayout;
	};

	class ETHEREAL_API Material : public IAsset
	{
	public:
		Material() = default;
		~Material() = default;

		// Setters
		void SetDiffuseTexturePath(const std::string& path);
		void SetNormalTextureName(const std::string& name);
		void SetSpecularTextureName(const std::string& name);
		void SetMetallicTextureName(const std::string& name);
		void SetRoughnessTextureName(const std::string& name);
		void SetDiffuseColor(const DirectX::XMFLOAT3& color);
		void SetVertexShaderName(const std::string& name);
		void SetPixelShaderName(const std::string& name);
		void SetInputLayoutJson(const nlohmann::json& json);
	

		bool Initialize();	
		bool ResolveTextures();
		bool ResolveShaders();
		bool BuildInputLayoutFromJson();
		bool CreateInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc);

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
		ID3D11InputLayout* GetInputLayout()
		{
			return m_InputLayout.Get();
		}

		const std::string& GetDiffuseTexturePath() const;
		ID3D11ShaderResourceView* GetDiffuseTexture() const;
		ID3D11ShaderResourceView* GetNormalTexture() const;
		ID3D11ShaderResourceView* GetSpecularTexture() const;
		ID3D11ShaderResourceView* GetMetallicTexture() const;
		ID3D11ShaderResourceView* GetRoughnessTexture() const;
		const DirectX::XMFLOAT3& GetDiffuseColor() const;
	private:
		nlohmann::json m_InputLayoutJson;

		std::string m_DiffuseTexturePath;
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
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
	};
}