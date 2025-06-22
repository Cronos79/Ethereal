#include "Model.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Ethereal
{							
	Model::Model()
	{
	}

	Model::Model(std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh)
	{
		m_Material = std::move(material);
		m_Mesh = std::move(mesh);
	}

	Model::~Model()
	{

	}	

	bool Model::Initialize()
	{
		if (!m_Material)
		{
			m_Material = std::make_shared<Material>();
		}
		if (!m_Mesh)
		{
			m_Mesh = std::make_shared<Mesh>();
		}
		if (!m_Material->Initialize())
		{
			LOG_ERROR("Failed to initialize material");
			return false;
		}
		
		if (!m_Mesh->Initialize())
		{
			LOG_ERROR("Failed to initialize mesh");
			return false;
		}

		// Create the input layout		
		D3D11_INPUT_ELEMENT_DESC layout[] = // D3D11_APPEND_ALIGNED_ELEMENT
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		auto* vert = m_Material->GetVertexShader();
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		HRESULT hr = device->CreateInputLayout(
			layout,
			ARRAYSIZE(layout),
			vert->GetVertexShaderBuffer()->GetBufferPointer(),
			vert->GetVertexShaderBuffer()->GetBufferSize(),
			&m_InputLayout
		);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to create input layout: {}", hr);
			return false;
		}

		ID3D11DeviceContext* context = static_cast<ID3D11DeviceContext*>(EEContext::Get().GetContext());
		hr = m_ConstantBuffer.Initialize(device, context);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to initialize model constant buffer: {}", hr);
			return false;
		}

		return true;
	}

}