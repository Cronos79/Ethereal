#include "Assets/Model.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"
#include "Renderer/DX11/VertexBufferLayout.h"
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
			aiProcess_ConvertToLeftHanded);

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
				material->SetDiffuseColor({ color.r, color.g, color.b });
			}
			float opacity = 1.0f;
			if (AI_SUCCESS == aiMat->Get(AI_MATKEY_OPACITY, opacity))
			{
				material->SetAlpha(opacity);
			}
			else
			{
				material->SetAlpha(1.0f); // Default to fully opaque if not specified
			}

			auto ovr = m_MaterialOverrides.find(i);
			if (ovr != m_MaterialOverrides.end())
			{
				const MaterialOverride & override = ovr->second;
				material->SetVertexShaderName(override.vertexShader);
				material->SetPixelShaderName(override.pixelShader);
				material->SetDiffuseTexturePath(override.diffuseTexture);
				material->SetNormalTexturePath(override.normalTexture);
				material->SetSpecularTexturePath(override.specularTexture);
				material->SetMetallicTexturePath(override.metallicTexture);
				material->SetRoughnessTexturePath(override.roughnessTexture);
				material->SetAOTexturePath(override.aoTexture);
				material->SetInputLayoutJson(override.inputLayout);
			}
			else
			{
				material->SetVertexShaderName(m_VertexShaderName);
				material->SetPixelShaderName(m_PixelShaderName);
				material->SetDiffuseTexturePath(m_DiffuseTexturePath);
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
		}

		LOG_INFO("Model '{}' loaded with {} mesh(es)", path, m_Meshes.size());
		return true;
	}


	void Model::SetMaterialOverride(int materialIndex, const MaterialOverride & override)
	{
		m_MaterialOverrides[materialIndex] = override;
	}

	void Model::SetMaterials(std::vector<std::shared_ptr<Material>>&& materials)
	{
		m_Materials = std::move(materials);
	}

	const std::vector<std::shared_ptr<Material>>& Model::GetMaterials() const
	{
		return m_Materials;
	}

	std::shared_ptr<Ethereal::Material> Model::GetMaterial(size_t index) const
	{
		return (index < m_Materials.size()) ? m_Materials[index] : nullptr;
	}

	void Model::SetMeshes(std::vector<std::shared_ptr<Mesh>>&& meshes)
	{
		m_Meshes = std::move(meshes);
	}

	const std::vector<std::shared_ptr<Mesh>>& Model::GetMeshes() const
	{
		return m_Meshes;
	}

	std::shared_ptr<Mesh> Model::GetMesh(size_t index) const
	{
		return (index < m_Meshes.size()) ? m_Meshes[index] : nullptr;
	}
} 