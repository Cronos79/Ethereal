#pragma once
#include "IAsset.h"
#include "Core/EtherealIncludes.h"
#include <vector>
#include "Assets/Mesh.h"
#include "Assets/Material.h"
#include "Renderer/DX11/ConstantBuffer.h"
#include "Renderer/DX11/ConstantBufferTypes.h"

using namespace DirectX;

namespace Ethereal
{
	class ETHEREAL_API Model : public IAsset
	{
	public:		
		Model();
		Model(std::vector<std::shared_ptr<Material>> materials, std::vector<std::shared_ptr<Mesh>> meshes);
		virtual ~Model();

		bool LoadFromFile(const std::string& path); //Assimp loading

		ID3D11InputLayout* GetInputLayout()
		{
			return m_InputLayout.Get();
		}
		ConstantBuffer<CB_VS_vertexshader>& GetConstantBuffer()
		{
			return m_ConstantBuffer;
		}

		void SetMaterials(const std::vector<std::shared_ptr<Material>>& materials)
		{
			m_Materials = materials;
		}
		void SetMaterials(std::vector<std::shared_ptr<Material>>&& materials)
		{
			m_Materials = std::move(materials);
		}
		const std::vector<std::shared_ptr<Material>>& GetMaterials() const
		{
			return m_Materials;
		}
		std::shared_ptr<Material> GetMaterial(size_t index) const
		{
			return (index < m_Materials.size()) ? m_Materials[index] : nullptr;
		}

		void SetMeshes(const std::vector<std::shared_ptr<Mesh>>& meshes)
		{
			m_Meshes = meshes;
		}
		void SetMeshes(std::vector<std::shared_ptr<Mesh>>&& meshes)
		{
			m_Meshes = std::move(meshes);
		}
		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const
		{
			return m_Meshes;
		}
		std::shared_ptr<Mesh> GetMesh(size_t index) const
		{
			return (index < m_Meshes.size()) ? m_Meshes[index] : nullptr;
		}
	
	private:
		bool CreateInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc);
		bool CreateConstantBuffer();	
		bool Init(); // Common initialization code

	private:
		std::vector<std::shared_ptr<Material>> m_Materials;
		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
		ConstantBuffer<CB_VS_vertexshader> m_ConstantBuffer;
		std::string m_TextureName = "Texture"; // Default texture name for materials without a specified texture
		std::string m_VertexShaderName = "VertexShader"; // Default vertex shader name
		std::string m_PixelShaderName = "PixelShader"; // Default pixel shader name
	};
}