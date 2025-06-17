#include "Assets/GameObject.h"
#include "Core/Logger.h"
#include "Core/EngineUtils.h"

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

		std::filesystem::path fullPath = GetAssetsDirectory();
		fullPath /= "";
		m_Material->Load(fullPath.string());

		return true;
	}

}