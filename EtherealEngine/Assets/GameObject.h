#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "Shaders.h"
#include "Material.h"
#include "Mesh.h"

namespace Ethereal
{
	class ETHEREAL_API GameObject : public IAsset
	{
	public:
		GameObject();
		~GameObject();

		bool Load(const std::string& path);

		
		// Assessors
		void SetMaterial(std::unique_ptr<Material> material)
		{
			m_Material = std::move(material);
		}
		Material* GetMaterial() const
		{
			return m_Material.get();
		}
		void SetMesh(std::unique_ptr<Mesh> mesh)
		{
			m_Mesh = std::move(mesh);
		}
		Mesh* GetMesh() const
		{
			return m_Mesh.get();
		}



	private:
		std::shared_ptr<Material> m_Material;
		std::shared_ptr<Mesh> m_Mesh;
	};
}