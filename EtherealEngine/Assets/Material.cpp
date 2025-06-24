#include "Assets/Material.h"
#include "Core/EEContext.h"
#include "Core/Logger.h"
#include "Renderer/DX11/RendererDX11.h"
#include "Core/EngineUtils.h"
#include <WICTextureLoader.h>
#include <cstring>

namespace Ethereal
{


	void Material::SetDiffuseTexturePath(const std::string& path)
	{
		m_DiffuseTexturePath = path;
	}

	void Material::SetNormalTexturePath(const std::string& path)
	{
		m_NormalTexturePath = path;
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

	void Material::SetInputLayoutJson(const nlohmann::json& json)
	{
		m_InputLayoutJson = json;
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

		if (!BuildInputLayoutFromJson())
		{
			LOG_ERROR("Failed to build input layout from JSON");
			return false;
		}

		return true;
	}

	bool Material::ResolveTextures()
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		// Get the file path with GetAssetsDirectory()
		if (!m_DiffuseTexturePath.empty())
		{
			std::filesystem::path texturePath = GetAssetsDirectory() / m_DiffuseTexturePath;
			std::wstring wpath = StringToWChar(texturePath.string());

			HRESULT hr = DirectX::CreateWICTextureFromFile(device, texturePath.c_str(), nullptr, m_DiffuseTextureView.GetAddressOf());

			if (FAILED(hr))
			{
				LOG_ERROR("Failed to create WIC DiffuseTexture from file: {}", hr);
				return false;
			}
		}
		if (!m_NormalTexturePath.empty())
		{
			std::filesystem::path texturePath = GetAssetsDirectory() / m_NormalTexturePath;
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringToWChar(texturePath.string()).c_str(), nullptr, m_NormalTextureView.GetAddressOf());
			if (FAILED(hr))
			{
				LOG_ERROR("Failed to create WIC NormalTexture from file: {}", hr);
			}
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

	bool Material::BuildInputLayoutFromJson()
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;
		std::vector<std::string> stableNames;
		stableNames.reserve(m_InputLayoutJson.size()); // avoid reallocations

		if (!m_InputLayoutJson.empty())
		{
			for (const auto& elem : m_InputLayoutJson)
			{
				D3D11_INPUT_ELEMENT_DESC desc = {};

				// Push string and immediately get pointer
				stableNames.emplace_back(elem.value("semantic", "POSITION"));
				const char* semanticCStr = stableNames.back().c_str();
				desc.SemanticName = semanticCStr;

				desc.SemanticIndex = elem.value("index", 0);
				desc.Format = FormatStringToDXGI(elem.value("format", "R32G32B32_FLOAT"));
				desc.InputSlot = 0;

				int offset = elem.value("offset", -1);
				desc.AlignedByteOffset = (offset == -1) ? D3D11_APPEND_ALIGNED_ELEMENT : static_cast<UINT>(offset);

				desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				desc.InstanceDataStepRate = 0;

				layoutDesc.push_back(desc);
			}
		}
		else
		{
			layoutDesc = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
		}

		if (!CreateInputLayout(layoutDesc))
		{
			LOG_ERROR("Failed to create input layout");
			return false;
		}
		return true;
	}

	bool Material::CreateInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc)
	{
		auto vertexShaderBlob = GetVertexShader()->GetVertexShaderBuffer();
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		HRESULT hr = device->CreateInputLayout(
			layoutDesc.data(),
			static_cast<UINT>(layoutDesc.size()),
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize(),
			&m_InputLayout
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create input layout: {}", hr);
			return false;
		}
		return true;
	}

	const std::string& Material::GetDiffuseTexturePath() const
	{
		return m_DiffuseTexturePath;
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