#include "Assets/GameObject.h"
#include "Core/Logger.h"

namespace Ethereal
{
	int32_t GameObject::s_NextID = 0;

	GameObject::GameObject()
	{
		m_ID = s_NextID++;
	}

	GameObject::GameObject(const std::string& name, std::unique_ptr<Material> material, std::unique_ptr<Mesh> mesh)
	{
		m_Name = name;
		m_Material = std::move(material);
		m_Mesh = std::move(mesh);
		m_ID = s_NextID++;
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
		}
		auto* vert = m_Material->GetVertexShader();
		if (!m_Mesh->Initialize(vert))
		{
			LOG_ERROR("Failed to initialize mesh");
		}
	}

	GameObject::GameObject(const std::string& name)
	{
		m_Name = name;
		m_ID = s_NextID++;
	}

	GameObject::~GameObject()
	{

	}

	bool GameObject::Initialize()
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

	void GameObject::DrawUI(float deltaTime)
	{

	}

	void GameObject::Update(float deltaTime)
	{

	}

}