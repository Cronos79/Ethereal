#include "Assets/Material.h"
#include "Core/EEContext.h"
#include "Core/Logger.h"
#include "Renderer/DX11/RendererDX11.h"
#include "Core/EngineUtils.h"
#include <WICTextureLoader.h>

namespace Ethereal
{

	void Material::SetDiffuseTextureName(const std::string& name)
	{
		m_DiffuseTextureName = name;
	}

	void Material::SetNormalTextureName(const std::string& name)
	{
		m_NormalTextureName = name;
	}

	void Material::SetSpecularTextureName(const std::string& name)
	{
		m_SpecularTextureName = name;
	}

	void Material::SetMetallicTextureName(const std::string& name)
	{
		m_MetallicTextureName = name;
	}

	void Material::SetRoughnessTextureName(const std::string& name)
	{
		m_RoughnessTextureName = name;
	}

	void Material::SetDiffuseColor(const DirectX::XMFLOAT3& color)
	{
		m_DiffuseColor = color;
	}

	void Material::SetVertexShaderName(const std::string& name)
	{
		m_VertexShaderName = name;
	}

	void Material::SetPixelShaderName(const std::string& name)
	{
		m_PixelShaderName = name;
	}

	bool Material::Initialize()
	{	
		if (!ResolveShaders())
		{
			return false;
		}
		if (!ResolveTextures())
		{
			return false;
		}
	
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		ID3D11DeviceContext* context = static_cast<ID3D11DeviceContext*>(EEContext::Get().GetContext());
		HRESULT hr = m_PSConstantBuffer.Initialize(device, context);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to initialize pixel shader constant buffer: {}", hr);
			return false;
		}

		return true;
	}

	bool Material::ResolveTextures()
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		// Get the file path with GetAssetsDirectory()
		std::wstring texturePath = StringToWChar(GetAssetsDirectory().string()) + L"\\Textures\\Test.png";

		HRESULT hr = DirectX::CreateWICTextureFromFile(device, texturePath.c_str(), nullptr, m_DiffuseTextureView.GetAddressOf());

		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create WIC texture from file: {}", hr);
			return false;
		}
		return true;
	}

	bool Material::ResolveShaders()
	{
		// Load the shaders into the asset manager
		EEContext::Get().GetAssetManager().LoadShader(m_VertexShaderName, ShaderType::VERTEX_SHADER);
		EEContext::Get().GetAssetManager().LoadShader(m_PixelShaderName, ShaderType::PIXEL_SHADER);

		// Get the Vertex shader from the asset manager
		m_VertexShaderAsset = EEContext::Get().GetAssetManager().Get<Shaders>(m_VertexShaderName);
		if (!m_VertexShaderAsset)
		{
			LOG_ERROR("Vertex shader '{}' not loaded", m_VertexShaderName);
			return false;
		}
		m_VertexBlob = m_VertexShaderAsset->GetVertexShaderBuffer();

		// Get the Pixel shader from the asset manager
		m_PixelShaderAsset = EEContext::Get().GetAssetManager().Get<Shaders>(m_PixelShaderName);
		if (!m_PixelShaderAsset)
		{
			LOG_ERROR("Pixel shader '{}' not loaded", m_PixelShaderName);
			return false;
		}
		m_PixelBlob = m_PixelShaderAsset->GetPixelShaderBuffer();
		return true;
	}

	const std::string& Material::GetDiffuseTextureName() const
	{
		return m_DiffuseTextureName;
	}

	ID3D11ShaderResourceView* Material::GetDiffuseTexture() const
	{
		return m_DiffuseTextureView.Get();
	}

	ID3D11ShaderResourceView* Material::GetNormalTexture() const
	{
		return m_NormalTextureView.Get();
	}

	ID3D11ShaderResourceView* Material::GetSpecularTexture() const
	{
		return m_SpecularTextureView.Get();
	}

	ID3D11ShaderResourceView* Material::GetMetallicTexture() const
	{
		return m_MetallicTextureView.Get();
	}

	ID3D11ShaderResourceView* Material::GetRoughnessTexture() const
	{
		return m_RoughnessTextureView.Get();
	}

	const DirectX::XMFLOAT3& Material::GetDiffuseColor() const
	{
		return m_DiffuseColor;
	}

}