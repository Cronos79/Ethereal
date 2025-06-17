#include "Assets/GameObject.h"
#include "Core/Logger.h"

namespace Ethereal
{
	GameObject::GameObject()
	{
		
	}

	GameObject::~GameObject()
	{

	}

	bool GameObject::Load(const std::string& path)
	{
		m_Material = std::make_shared<Material>();
		m_Mesh = std::make_shared<Mesh>();

		if (!m_Material)
		{
			LOG_ERROR("Material is nullptr");
		}
		if (!m_Mesh)
		{
			LOG_ERROR("Mesh is nullptr");
		}
		if (!m_Material->Initialize())
		{
			LOG_ERROR("Failed to initialize material");
			return false;
		}
		auto* vert = m_Material->GetVertexShader();
		if (!m_Mesh->Initialize(vert))
		{
			LOG_ERROR("Failed to initialize mesh");
			return false;
		}

		return true;
	}
}