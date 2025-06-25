#include "Model.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"
#include "Renderer/DX11/VertexBufferLayout.h"

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

		// Load Materials
		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			const aiMaterial* aiMat = scene->mMaterials[i];
			auto material = std::make_shared<Material>();
			material->GetConstantBuffer().Initialize(device, nullptr);

			aiColor3D color(1.0f, 1.0f, 1.0f);
			if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
			{
				material->SetDiffuseColor({ color.r, color.g, color.b });
			}

			auto ovr = m_MaterialOverrides.find(i);
			if (ovr != m_MaterialOverrides.end())
			{
				material->SetVertexShaderName(ovr->second.vertexShader);
				material->SetPixelShaderName(ovr->second.pixelShader);
				material->SetDiffuseTexturePath(ovr->second.diffuseTexture);
				material->SetNormalTexturePath(ovr->second.normalTexture);
				material->SetInputLayoutJson(ovr->second.inputLayout);
			}
			else
			{
				material->SetVertexShaderName(m_VertexShaderName);
				material->SetPixelShaderName(m_PixelShaderName);
				material->SetDiffuseTexturePath(m_DiffuseTexturePath);
				material->SetNormalTexturePath(m_NormalTexturePath);
			}

			if (!material->Initialize())
			{
				LOG_ERROR("Failed to initialize material for model: {}", path);
				return false;
			}
			m_Materials.push_back(material);
		}

		// Load Meshes
		for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
		{
			const aiMesh* assimpMesh = scene->mMeshes[m];
			auto mesh = std::make_shared<Mesh>();
			mesh->SetName(assimpMesh->mName.C_Str());
			mesh->SetMaterialIndex(assimpMesh->mMaterialIndex);

			VertexBufferLayout layout;
			layout.AddAttribute(VertexAttributeType::Position, DXGI_FORMAT_R32G32B32_FLOAT, "POSITION");
			bool hasNormals = assimpMesh->HasNormals();
			bool hasTexCoords = assimpMesh->HasTextureCoords(0);
			bool hasTangents = assimpMesh->HasTangentsAndBitangents();
			if (hasNormals)
				layout.AddAttribute(VertexAttributeType::Normal, DXGI_FORMAT_R32G32B32_FLOAT, "NORMAL");
			if (hasTexCoords)
				layout.AddAttribute(VertexAttributeType::TexCoord, DXGI_FORMAT_R32G32_FLOAT, "TEXCOORD");
			if (hasTangents)
				layout.AddAttribute(VertexAttributeType::Tangent, DXGI_FORMAT_R32G32B32_FLOAT, "TANGENT");

			layout.Build();
			UINT stride = layout.GetStride();

			std::vector<uint8_t> vertexData(assimpMesh->mNumVertices * stride);
			for (unsigned int i = 0; i < assimpMesh->mNumVertices; ++i)
			{
				uint8_t* dst = vertexData.data() + i * stride;

				// Position
				memcpy(dst, &assimpMesh->mVertices[i], sizeof(float) * 3);
				UINT offset = sizeof(float) * 3;

				if (hasNormals)
				{
					memcpy(dst + offset, &assimpMesh->mNormals[i], sizeof(float) * 3);
					offset += sizeof(float) * 3;
				}

				if (hasTexCoords)
				{
					float uv[2] = {
						assimpMesh->mTextureCoords[0][i].x,
						assimpMesh->mTextureCoords[0][i].y
					};
					memcpy(dst + offset, uv, sizeof(float) * 2);
					offset += sizeof(float) * 2;
				}

				if (hasTangents)
				{
					memcpy(dst + offset, &assimpMesh->mTangents[i], sizeof(float) * 3);
					offset += sizeof(float) * 3;
				}
			}

			std::vector<uint32_t> indices;
			for (unsigned int i = 0; i < assimpMesh->mNumFaces; ++i)
			{
				const aiFace& face = assimpMesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; ++j)
					indices.push_back(face.mIndices[j]);
			}

			mesh->Initialize(std::move(vertexData), std::move(layout), std::move(indices));
			m_Meshes.push_back(mesh);

			/*LOG_INFO("Loaded mesh '{}' with {} verts, {} indices, material {}",
				mesh->GetName(), assimpMesh->mNumVertices, indices.size(), mesh->GetMaterialIndex());*/
		}

		if (!CreateConstantBuffer())
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