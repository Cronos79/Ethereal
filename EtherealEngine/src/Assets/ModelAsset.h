#pragma once
#pragma once
#include <string>
#include "Assets/Mesh.h"
#include <assimp/scene.h>
#include "Assets/Material.h"
#include "IAsset.h"

namespace EtherealEngine
{
	class ModelAsset : public IAsset
	{
	public:
		ModelAsset() = default;
		virtual ~ModelAsset() override = default;

		bool LoadFromFile(const std::string& path);

		const std::vector<Mesh>& GetMeshes() const
		{
			return m_Meshes;
		}
		const std::vector<Material>& GetMaterials() const
		{
			return m_Materials;
		}

	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		void LoadMaterials(const aiScene* scene);

	private:
		std::vector<Mesh> m_Meshes; // Store multiple meshes
		std::vector<Material> m_Materials;

		// Future: Add accessors for mesh/material data
	};
}