#include "pch.h"
#include "ModelAsset.h"
#include "./Core/EELoggerMacros.h"

#include <assimp/Importer.hpp>

#include <assimp/postprocess.h>

namespace EtherealEngine
{
	bool ModelAsset::LoadFromFile(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_FlipUVs
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_ERROR("Assimp failed to load model '{}': {}", path, importer.GetErrorString());
			return false;
		}

		LOG_INFO("Successfully loaded model: {}", path);
		LOG_INFO(" - Meshes: {}", scene->mNumMeshes);
		LOG_INFO(" - Materials: {}", scene->mNumMaterials);

		m_Meshes.clear();
		m_Materials.clear();

		LoadMaterials(scene);          
		ProcessNode(scene->mRootNode, scene);

		return true;
	}

	void ModelAsset::LoadMaterials(const aiScene* scene)
	{
		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			const aiMaterial* aiMat = scene->mMaterials[i];
			Material material;

			// Diffuse color
			aiColor3D color(1.0f, 1.0f, 1.0f);
			if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
			{
				material.SetDiffuseColor({ color.r, color.g, color.b });
			}

			// Specular color
			if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
			{
				material.SetSpecularColor({ color.r, color.g, color.b });
			}

			// Emissive color
			if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
			{
				material.SetEmissiveColor({ color.r, color.g, color.b });
			}

			// Shininess
			float shininess = 32.0f;
			if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
			{
				material.SetShininess(shininess);
			}

			// Textures
			aiString texPath;

			if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
			{
				material.SetDiffuseTexture(texPath.C_Str());
			}
			if (aiMat->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS)
			{
				material.SetSpecularTexture(texPath.C_Str());
			}
			if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS ||
				aiMat->GetTexture(aiTextureType_HEIGHT, 0, &texPath) == AI_SUCCESS) // fallback
			{
				material.SetNormalTexture(texPath.C_Str());
			}

			// Optional logging
			LOG_INFO("Material [{}]", i);
			LOG_INFO("  Diffuse:  ({}, {}, {})", material.GetDiffuseColor().x, material.GetDiffuseColor().y, material.GetDiffuseColor().z);
			LOG_INFO("  Specular: ({}, {}, {})", material.GetSpecularColor().x, material.GetSpecularColor().y, material.GetSpecularColor().z);
			LOG_INFO("  Emissive: ({}, {}, {})", material.GetEmissiveColor().x, material.GetEmissiveColor().y, material.GetEmissiveColor().z);
			LOG_INFO("  Shininess: {}", material.GetShininess());
			LOG_INFO("  Textures:");
			LOG_INFO("    Diffuse:  {}", material.GetDiffuseTexture().empty() ? "none" : material.GetDiffuseTexture());
			LOG_INFO("    Specular: {}", material.GetSpecularTexture().empty() ? "none" : material.GetSpecularTexture());
			LOG_INFO("    Normal:   {}", material.GetNormalTexture().empty() ? "none" : material.GetNormalTexture());

			m_Materials.push_back(material);
		}
	}

	void ModelAsset::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh ModelAsset::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		Mesh result;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertex vertex;

			vertex.position = {
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			};

			if (mesh->HasNormals())
			{
				vertex.normal = {
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.texcoord = {
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
			}
			else
			{
				vertex.texcoord = { 0.0f, 0.0f };
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		result.SetVertices(vertices);
		result.SetIndices(indices);
		result.SetMaterialIndex(mesh->mMaterialIndex);

		return result;
	}
}