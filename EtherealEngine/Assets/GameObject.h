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
#include "Model.h"

namespace Ethereal
{
	class ETHEREAL_API GameObject : public IAsset
	{
	public:
		GameObject();
		GameObject(const std::string& name);
		~GameObject();

		virtual void DrawUI(float deltaTime);
		virtual void Update(float deltaTime);
		
		// Assessors
		
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
		std::shared_ptr<Model> GetModelPtr()
		{
			return m_Model;
		}
		Model& GetModel()
		{
			return *m_Model;
		}
		void SetModel(const std::shared_ptr<Model>& model)
		{
			m_Model = model;
		}

		bool operator==(const GameObject& other) const
		{
			// Compare by unique ID, name, or whatever makes sense for your game objects
			return this->GetID() == other.GetID();
		}
	private:	
		std::shared_ptr<Model> m_Model;
		bool m_IsVisible = true;
		std::string m_Name;
		int32_t m_ID = 0;
		static int32_t s_NextID;
	};
}