#pragma once
#include "IAsset.h"
#include "Core/EtherealIncludes.h"
#include <vector>
#include <unordered_map>
#include "Assets/Mesh.h"
#include "Assets/Material.h"
#include "Renderer/DX11/ConstantBuffer.h"
#include "Renderer/DX11/ConstantBufferTypes.h"

namespace Ethereal
{
	class ETHEREAL_API Model : public IAsset
	{
	public:
		Model() = default;
		virtual ~Model() = default;

		bool LoadFromFile(const std::string& path);

		void SetMaterials(std::vector<std::shared_ptr<Material>>&& materials);
		const std::vector<std::shared_ptr<Material>>& GetMaterials() const;
		std::shared_ptr<Material> GetMaterial(size_t index) const;

		void SetMeshes(std::vector<std::shared_ptr<Mesh>>&& meshes);
		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const;
		std::shared_ptr<Mesh> GetMesh(size_t index) const;

		void SetVertexShaderName(const std::string& name);
		void SetPixelShaderName(const std::string& name);
		void SetDiffuseTexturePath(const std::string& path);
		void SetNormalTexturePath(const std::string& path);
		void SetMaterialOverride(int materialIndex, const MaterialOverride & override);

	private:

		std::vector<std::shared_ptr<Material>> m_Materials;
		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		std::unordered_map<int, MaterialOverride> m_MaterialOverrides;

		std::string m_VertexShaderName = "VertexShader";
		std::string m_PixelShaderName = "PixelShader";
		std::string m_DiffuseTexturePath = "Textures/Test.png";
		std::string m_NormalTexturePath = "Textures/Normal.png";
	};
}