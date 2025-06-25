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
	void Material::SetSpecularTexturePath(const std::string& path)
	{
		m_SpecularTexturePath = path;
	}
	void Material::SetMetallicTexturePath(const std::string& path)
	{
		m_MetallicTexturePath = path;
	}
	void Material::SetRoughnessTexturePath(const std::string& path)
	{
		m_RoughnessTexturePath = path;
	}
	void Material::SetAOTexturePath(const std::string& path)
	{
		m_AOTexturePath = path;
	}
	void Material::SetEmissiveTexturePath(const std::string& path)
	{
		m_EmissiveTexturePath = path;
	}
	void Material::SetDiffuseColor(const DirectX::XMFLOAT3& color)
	{
		m_DiffuseColor = color;
	}
	void Material::SetEmissiveColor(const DirectX::XMFLOAT3& color)
	{
		m_EmissiveColor = color;
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
	void Material::SetMetallicValue(float value)
	{
		m_MetallicValue = value;
	}
	void Material::SetRoughnessValue(float value)
	{
		m_RoughnessValue = value;
	}
	void Material::SetSpecularStrength(float strength)
	{
		m_SpecularStrength = strength;
	}

	bool Material::Initialize()
	{
		if (!ResolveShaders() || !ResolveTextures()) return false;

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
		auto load = [&](const std::string& path, auto& view, bool& hasFlag) -> bool {
			if (path.empty()) return false;
			auto fullPath = GetAssetsDirectory() / path;
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringToWChar(fullPath.string()).c_str(), nullptr, view.GetAddressOf());
			hasFlag = SUCCEEDED(hr);
			if (!hasFlag) LOG_ERROR("Failed to load texture: {}", fullPath.string());
			return hasFlag;
			};

		load(m_DiffuseTexturePath, m_DiffuseTextureView, m_HasDiffuse);
		load(m_NormalTexturePath, m_NormalTextureView, m_HasNormal);
		load(m_SpecularTexturePath, m_SpecularTextureView, m_HasSpecular);
		load(m_MetallicTexturePath, m_MetallicTextureView, m_HasMetallic);
		load(m_RoughnessTexturePath, m_RoughnessTextureView, m_HasRoughness);
		load(m_AOTexturePath, m_AOTextureView, m_HasAO);
		load(m_EmissiveTexturePath, m_EmissiveTextureView, m_HasEmissive);
		return true;
	}

	bool Material::ResolveShaders()
	{
		auto& manager = EEContext::Get().GetAssetManager();
		manager.LoadShader(m_VertexShaderName, ShaderType::VERTEX_SHADER);
		manager.LoadShader(m_PixelShaderName, ShaderType::PIXEL_SHADER);

		m_VertexShaderAsset = manager.Get<Shaders>(m_VertexShaderName);
		m_VertexBlob = m_VertexShaderAsset ? m_VertexShaderAsset->GetVertexShaderBuffer() : nullptr;
		m_PixelShaderAsset = manager.Get<Shaders>(m_PixelShaderName);
		m_PixelBlob = m_PixelShaderAsset ? m_PixelShaderAsset->GetPixelShaderBuffer() : nullptr;

		if (!m_VertexShaderAsset || !m_PixelShaderAsset)
		{
			LOG_ERROR("Failed to resolve shaders: VS '{}', PS '{}'", m_VertexShaderName, m_PixelShaderName);
			return false;
		}
		return true;
	}

	bool Material::BuildInputLayoutFromJson()
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;
		std::vector<std::string> stableNames;
		stableNames.reserve(m_InputLayoutJson.size());

		if (!m_InputLayoutJson.empty())
		{
			for (const auto& elem : m_InputLayoutJson)
			{
				D3D11_INPUT_ELEMENT_DESC desc = {};
				stableNames.emplace_back(elem.value("semantic", "POSITION"));
				desc.SemanticName = stableNames.back().c_str();
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
		return CreateInputLayout(layoutDesc);
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
			&m_InputLayout);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create input layout: {}", hr);
			return false;
		}
		return true;
	}

	Ethereal::Shaders* Material::GetVertexShader()
	{
		return m_VertexShaderAsset.get();
	}

	Ethereal::Shaders* Material::GetPixelShader()
	{
		return m_PixelShaderAsset.get();
	}

	ConstantBuffer<CB_PS_pixelshader>& Material::GetConstantBuffer()
	{
		return m_PSConstantBuffer;
	}

	ID3D11InputLayout* Material::GetInputLayout()
	{
		return m_InputLayout.Get();
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
	ID3D11ShaderResourceView* Material::GetAOTexture() const
	{
		return m_AOTextureView.Get();
	}
	ID3D11ShaderResourceView* Material::GetEmissiveTexture() const
	{
		return m_EmissiveTextureView.Get();
	}
	const DirectX::XMFLOAT3& Material::GetDiffuseColor() const
	{
		return m_DiffuseColor;
	}
	const DirectX::XMFLOAT3& Material::GetEmissiveColor() const
	{
		return m_EmissiveColor;
	}

	bool Material::HasDiffuseMap() const
	{
		return m_HasDiffuse;
	}

	bool Material::HasNormalMap() const
	{
		return m_HasNormal;
	}

	bool Material::HasMetallicMap() const
	{
		return m_HasMetallic;
	}

	bool Material::HasRoughnessMap() const
	{
		return m_HasRoughness;
	}

	bool Material::HasSpecularMap() const
	{
		return m_HasSpecular;
	}

	bool Material::HasAOMap() const
	{
		return m_HasAO;
	}

	bool Material::HasEmissiveMap() const
	{
		return m_HasEmissive;
	}

	float Material::GetMetallicValue() const
	{
		return m_MetallicValue;
	}
	float Material::GetRoughnessValue() const
	{
		return m_RoughnessValue;
	}
	float Material::GetSpecularStrength() const
	{
		return m_SpecularStrength;
	}
} // namespace Ethereal
