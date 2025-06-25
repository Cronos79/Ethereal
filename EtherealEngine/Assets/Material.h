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
		std::string normalTexture = "";
		std::string specularTexture = "";
		std::string metallicTexture = "";
		std::string roughnessTexture = "";
		std::string aoTexture = "";
		nlohmann::json inputLayout;
	};

	class ETHEREAL_API Material : public IAsset
	{
	public:
		Material() = default;
		~Material() = default;

		// Setters
		void SetDiffuseTexturePath(const std::string& path);
		void SetNormalTexturePath(const std::string& path);
		void SetSpecularTexturePath(const std::string& path);
		void SetMetallicTexturePath(const std::string& path);
		void SetRoughnessTexturePath(const std::string& path);
		void SetAOTexturePath(const std::string& path);
		void SetEmissiveTexturePath(const std::string& path);
		void SetDiffuseColor(const DirectX::XMFLOAT3& color);
		void SetEmissiveColor(const DirectX::XMFLOAT3& color);
		void SetVertexShaderName(const std::string& name);
		void SetPixelShaderName(const std::string& name);
		void SetInputLayoutJson(const nlohmann::json& json);
		void SetMetallicValue(float value);
		void SetRoughnessValue(float value);
		void SetSpecularStrength(float strength);

		bool Initialize();
		bool ResolveTextures();
		bool ResolveShaders();
		bool BuildInputLayoutFromJson();
		bool CreateInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc);

		Shaders* GetVertexShader();
		Shaders* GetPixelShader();
		ConstantBuffer<CB_PS_pixelshader>& GetConstantBuffer();
		ID3D11InputLayout* GetInputLayout();

		const std::string& GetDiffuseTexturePath() const;
		ID3D11ShaderResourceView* GetDiffuseTexture() const;
		ID3D11ShaderResourceView* GetNormalTexture() const;
		ID3D11ShaderResourceView* GetSpecularTexture() const;
		ID3D11ShaderResourceView* GetMetallicTexture() const;
		ID3D11ShaderResourceView* GetRoughnessTexture() const;
		ID3D11ShaderResourceView* GetAOTexture() const;
		ID3D11ShaderResourceView* GetEmissiveTexture() const;
		const DirectX::XMFLOAT3& GetDiffuseColor() const;
		const DirectX::XMFLOAT3& GetEmissiveColor() const;

		bool HasDiffuseMap() const;
		bool HasNormalMap() const;
		bool HasMetallicMap() const;
		bool HasRoughnessMap() const;
		bool HasSpecularMap() const;
		bool HasAOMap() const;
		bool HasEmissiveMap() const;
		float GetMetallicValue() const;
		float GetRoughnessValue() const;
		float GetSpecularStrength() const;

	private:
		nlohmann::json m_InputLayoutJson;

		std::string m_DiffuseTexturePath;
		std::string m_NormalTexturePath;
		std::string m_SpecularTexturePath;
		std::string m_MetallicTexturePath;
		std::string m_RoughnessTexturePath;
		std::string m_AOTexturePath;
		std::string m_EmissiveTexturePath;

		DirectX::XMFLOAT3 m_DiffuseColor = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 m_EmissiveColor = { 0.0f, 0.0f, 0.0f };

		float m_MetallicValue = 1.0f;
		float m_RoughnessValue = 1.0f;
		float m_SpecularStrength = 0.5f;

		bool m_HasDiffuse = false;
		bool m_HasNormal = false;
		bool m_HasMetallic = false;
		bool m_HasRoughness = false;
		bool m_HasSpecular = false;
		bool m_HasAO = false;
		bool m_HasEmissive = false;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_DiffuseTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_NormalTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SpecularTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_MetallicTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_RoughnessTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_AOTextureView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_EmissiveTextureView;

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