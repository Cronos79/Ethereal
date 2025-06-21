#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include "Shaders.h"
#include "Material.h"
#include "Mesh.h"
#include <stdint.h>

namespace Ethereal
{
	class ETHEREAL_API GameObject : public IAsset
	{
	public:
		GameObject();
		GameObject(const std::string& name);
		GameObject(const std::string& name, std::unique_ptr<Material> material, std::unique_ptr<Mesh> mesh);
		~GameObject();

		bool Initialize();
		virtual void DrawUI(float deltaTime);
		virtual void Update(float deltaTime);
		
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
		bool IsVisible() const
		{
			return m_IsVisible;
		}
		void SetVisible(bool visible)
		{
			m_IsVisible = visible;
		}
		const std::string& GetName() const
		{
			return m_Name;
		}
		void SetName(const std::string& name)
		{
			m_Name = name;
		}
		int32_t GetID() const
		{
			return m_ID;
		}
		void NewID()
		{
			m_ID = s_NextID++;
		}

		bool operator==(const GameObject& other) const
		{
			// Compare by unique ID, name, or whatever makes sense for your game objects
			return this->GetID() == other.GetID();
		}
	private:
		std::shared_ptr<Material> m_Material;
		std::shared_ptr<Mesh> m_Mesh;
		bool m_IsVisible = true;
		std::string m_Name;
		int32_t m_ID = 0;
		static int32_t s_NextID;
	};
}