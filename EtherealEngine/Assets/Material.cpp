#include "Assets/Material.h"
#include "Core/EEContext.h"
#include "Core/Logger.h"
#include "Renderer/DX11/RendererDX11.h"

namespace Ethereal
{
	Material::Material()
	{

	}

	Material::~Material()
	{

	}

	bool Material::Load(const std::string& path)
	{
		if (path.empty())
		{
			return false;
		}


		// Load the shaders into the asset manager
		EEContext::Get().GetAssetManager().LoadShader("VertexShader", ShaderType::VERTEX_SHADER);
		EEContext::Get().GetAssetManager().LoadShader("PixelShader", ShaderType::PIXEL_SHADER);

		// D3D11_APPEND_ALIGNED_ELEMENT
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// Get the Vertex shader from the asset manager
		m_VertexShaderAsset = EEContext::Get().GetAssetManager().Get<Shaders>("VertexShader");
		m_VertexBlob = m_VertexShaderAsset->GetVertexShaderBuffer();

		// Get the Pixel shader from the asset manager
		m_PixelShaderAsset = EEContext::Get().GetAssetManager().Get<Shaders>("PixelShader");
		m_PixelBlob = m_PixelShaderAsset->GetPixelShaderBuffer();

		// Get the Device from the Renderer
		auto device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());

		// Create the input layout
		HRESULT hr = device->CreateInputLayout(
			layout,
			ARRAYSIZE(layout),
			m_VertexBlob->GetBufferPointer(), // Shader bytecode will be provided later
			m_VertexBlob->GetBufferSize(), // Size of the shader bytecode
			&m_InputLayout // Output input layout
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create input layout: {}", hr);
			return false;
		}

		InitializeBuffers();

		return true;
	}

	bool Material::InitializeBuffers()
	{
		Vertex v[] =
		{
			Vertex(0.0f, 0.0f), // Position
		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData = {};
		vertexBufferData.pSysMem = v;

		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		HRESULT hr = device->CreateBuffer(&bufferDesc, &vertexBufferData, &m_VertexBuffer);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create vertex buffer: {}", hr);
			return false;
		}
	}

	ID3D11InputLayout* Material::GetInputLayout()
	{
		return m_InputLayout.Get();
	}
}