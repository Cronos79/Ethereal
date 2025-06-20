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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
		//Textured Square
		Vertex v[] =
		{
			Vertex(-0.5f,  -0.5f, 0.0f, 0.0f, 1.0f), //Bottom Left 0
			Vertex(-0.5f,   0.5f, 0.0f, 0.0f, 0.0f), //Top Left 1
			Vertex(0.5f,   0.5f, 0.0f, 1.0f, 0.0f), //Top Right 2
			Vertex(0.5f,  -0.5f, 0.0f, 1.0f, 1.0f), //Bottom Right 3
		};

		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		ID3D11DeviceContext* context = static_cast<ID3D11DeviceContext*>(EEContext::Get().GetContext());
		HRESULT hr = m_VertexBuffer.Initialize(device, v, ARRAYSIZE(v));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create vertex buffer: {}", hr);
			return false;
		}

		DWORD indices[] =
		{
			0, 1, 2, // First triangle
			0, 2, 3  // Second triangle
		};

		hr = m_IndexBuffer.Initialize(device, indices, ARRAYSIZE(indices));
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create index buffer: {}", hr);
			return false;
		}

		// Initialize the Constant Buffer
		hr = m_ConstantBuffer.Initialize(device, context);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create constant buffer: {}", hr);
			return false;
		}

		return true;
	}

	void Mesh::Update(DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix)
	{
		// Store the final matrix (world * view * projection), transposed for HLSL
		GetConstantBuffer().data.mat = DirectX::XMMatrixTranspose(worldMatrix * viewMatrix * projectionMatrix);

		if (!GetConstantBuffer().ApplyChanges())
		{
			LOG_ERROR("Failed to apply changes to constant buffer");
			return;
		}
	}

	ID3D11InputLayout* Mesh::GetInputLayout()
	{
		return m_InputLayout.Get();
	}

}