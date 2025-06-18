#include "Assets/Mesh.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"

namespace Ethereal
{
	bool Mesh::Initialize(Shaders* vertexShaderAsset)
	{
		// D3D11_APPEND_ALIGNED_ELEMENT
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		// Get the Device from the Renderer
		auto device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		
		// Create the input layout
		HRESULT hr = device->CreateInputLayout(
			layout,
			ARRAYSIZE(layout),
			vertexShaderAsset->GetVertexShaderBuffer()->GetBufferPointer(), // Shader bytecode 
			vertexShaderAsset->GetVertexShaderBuffer()->GetBufferSize(), // Size of the shader bytecode
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

	bool Mesh::InitializeBuffers()
	{
		Vertex v[] =
		{
			Vertex(-0.5f, -0.5f, 1.0f, 0.0f, 0.0f), // Center
			Vertex(0.0f, 0.5f, 0.0f, 1.0f, 0.0f), // Left
			Vertex(0.5f, -0.5f, 0.0f, 0.0f, 1.0f), // Right
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

	ID3D11InputLayout* Mesh::GetInputLayout()
	{
		return m_InputLayout.Get();
	}

}