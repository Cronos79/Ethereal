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
		Init();
	}

	Model::Model(std::vector<std::shared_ptr<Material>> materials, std::vector<std::shared_ptr<Mesh>> meshes)
	{
		m_Materials = std::move(materials);
		m_Meshes = std::move(meshes);
		Init();
	}

	Model::~Model()
	{

	}

	bool Model::LoadFromFile(const std::string& path)
	{
		auto assetManager = EEContext::Get().GetAssetManager();
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		m_Meshes.clear();
		m_Materials.clear();

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded
		);

		if (!scene || !scene->HasMeshes())
		{
			LOG_ERROR("Failed to load model: {}", path);
			return false;
		}

		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			const aiMaterial* aiMat = scene->mMaterials[i];
			auto material = std::make_shared<Material>();
			material->GetConstantBuffer().Initialize(device, nullptr);

			aiColor3D color(1.0f, 1.0f, 1.0f);
			if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
			{
				const DirectX::XMFLOAT3 colorF3(color.r, color.g, color.b);
				material->SetDiffuseColor(colorF3);
			}

			if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString path;
				if (AI_SUCCESS == aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path))
				{
					material->SetDiffuseTextureName(m_TextureName);
					// You'll handle loading the texture in Material later
				}
			}

			material->SetVertexShaderName("VertexShader");
			material->SetPixelShaderName("PixelShader");
			material->Initialize();
			material->ResolveTextures();
			m_Materials.push_back(material);
			material->SetVertexShaderName("VertexShader");
			material->SetPixelShaderName("PixelShader");
			if (!material->ResolveShaders())
			{
				if (!material->GetVertexShader())
					LOG_ERROR("Material {} missing vertex shader!", i);
				if (!material->GetPixelShader())
					LOG_ERROR("Material {} missing pixel shader!", i);
			}
		}

		for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
		{
			const aiMesh* assimpMesh = scene->mMeshes[m];
			auto mesh = std::make_shared<Mesh>();
			mesh->SetName(assimpMesh->mName.C_Str());
			mesh->SetMaterialIndex(assimpMesh->mMaterialIndex);

			mesh->GetVertices().reserve(assimpMesh->mNumVertices);
			for (unsigned int i = 0; i < assimpMesh->mNumVertices; ++i)
			{
				Vertex vertex;
				vertex.pos = DirectX::XMFLOAT3(
					assimpMesh->mVertices[i].x,
					assimpMesh->mVertices[i].y,
					assimpMesh->mVertices[i].z
				);
				vertex.normal = DirectX::XMFLOAT3(
					assimpMesh->mNormals[i].x,
					assimpMesh->mNormals[i].y,
					assimpMesh->mNormals[i].z
				);
				if (assimpMesh->HasTextureCoords(0))
				{
					vertex.texCoord = DirectX::XMFLOAT2(
						assimpMesh->mTextureCoords[0][i].x,
						assimpMesh->mTextureCoords[0][i].y
					);
				}
				else
				{
					vertex.texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
				}
				mesh->GetVertices().push_back(vertex);
			}

			for (unsigned int i = 0; i < assimpMesh->mNumFaces; ++i)
			{
				const aiFace& face = assimpMesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; ++j)
					mesh->GetIndices().push_back(face.mIndices[j]);
			}

			mesh->Initialize(mesh->GetVertices(), mesh->GetIndices());
			m_Meshes.push_back(mesh);			

			LOG_INFO("Loaded mesh '{}' with {} vertices, {} indices, material {}",
				mesh->GetName(), mesh->GetVertices().size(), mesh->GetIndices().size(), mesh->GetMaterialIndex());
		}

		LOG_INFO("Model '{}' loaded with {} mesh(es)", path, m_Meshes.size());

		return true;
	}

	bool Model::CreateInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc)
	{
		auto vertexShaderBlob = m_Materials[0]->GetVertexShader()->GetVertexShaderBuffer();
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

	bool Model::CreateConstantBuffer()
	{
		ID3D11Device* device = static_cast<ID3D11Device*>(EEContext::Get().GetDevice());
		ID3D11DeviceContext* context = static_cast<ID3D11DeviceContext*>(EEContext::Get().GetContext());
		HRESULT hr = m_ConstantBuffer.Initialize(device, context);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to initialize model constant buffer: {}", hr);
			return false;
		}
		return true;
	}

	bool Model::Init()
	{
		bool result = false;
		if (m_Materials.empty())
		{
			auto mat = std::make_shared<Material>();
			m_Materials.push_back(mat);
		}
		if (m_Meshes.empty())
		{
			m_Meshes.push_back(std::make_shared<Mesh>());
		}
		m_Materials[0]->SetVertexShaderName("VertexShader");
		m_Materials[0]->SetPixelShaderName("PixelShader");
		if (!m_Materials[0]->Initialize())
		{
			LOG_ERROR("Failed to initialize material");
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		result = CreateInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>(std::begin(layout), std::end(layout)));
		if (!result)
		{
			LOG_ERROR("Failed to create input layout");
			return false;
		}

		result = CreateConstantBuffer();
		if (!result)
		{
			LOG_ERROR("Failed to create constant buffer");
			return false;
		}

		return result;
	}

	void Model::TEMPSetup()
	{
		std::vector<Vertex> vertices = {
			// Front face (z = -0.5, normal = (0, 0, -1))
			{ -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f }, // 0
			{ -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f }, // 1
			{  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f }, // 2
			{  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f }, // 3

			// Back face (z = 0.5, normal = (0, 0, 1))
			{  0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f },  // 4
			{  0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f },  // 5
			{ -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f },  // 6
			{ -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f },  // 7

			// Left face (x = -0.5, normal = (-1, 0, 0))
			{ -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f }, // 8
			{ -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f }, // 9
			{ -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f }, // 10
			{ -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f }, // 11

			// Right face (x = 0.5, normal = (1, 0, 0))
			{ 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f },  // 12
			{ 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f },  // 13
			{ 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f },  // 14
			{ 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f },  // 15

			// Top face (y = 0.5, normal = (0, 1, 0))
			{ -0.5f, 0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f },  // 16
			{ -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f },  // 17
			{  0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f },  // 18
			{  0.5f, 0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f },  // 19

			// Bottom face (y = -0.5, normal = (0, -1, 0))
			{ -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f }, // 20
			{ -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f }, // 21
			{  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f }, // 22
			{  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f }, // 23
		};

		std::vector<uint32_t> indices = {
			// Front face
			0, 1, 2, 0, 2, 3,
			// Back face
			4, 5, 6, 4, 6, 7,
			// Left face
			8, 9,10, 8,10,11,
			// Right face
		   12,13,14,12,14,15,
		   // Top face
		  16,17,18,16,18,19,
		  // Bottom face
		 20,21,22,20,22,23
		};

		if (!m_Meshes[0]->Initialize(vertices, indices))
		{
			LOG_ERROR("Failed to initialize mesh");
		}
	}
}