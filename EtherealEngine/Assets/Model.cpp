#include "Model.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Ethereal
{
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

			auto ovr = m_MaterialOverrides.find(i);
			if (ovr != m_MaterialOverrides.end())
			{
				material->SetVertexShaderName(ovr->second.vertexShader);
				material->SetPixelShaderName(ovr->second.pixelShader);
				material->SetDiffuseTexturePath(ovr->second.diffuseTexture);
				material->SetInputLayoutJson(ovr->second.inputLayout);
			}
			else
			{
				// Use defaults if override not provided
				material->SetVertexShaderName("VertexShader");
				material->SetPixelShaderName("PixelShader");
				material->SetDiffuseTexturePath("Textures/UV.png");
			}

			if (!material->Initialize())
			{
				LOG_ERROR("Failed to initialize material for model: {}", path);
				return false;
			}
			m_Materials.push_back(material);
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

		if(!CreateConstantBuffer())
		{
			LOG_ERROR("Failed to create constant buffer for model: {}", path);
			return false;
		}


		LOG_INFO("Model '{}' loaded with {} mesh(es)", path, m_Meshes.size());

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
}