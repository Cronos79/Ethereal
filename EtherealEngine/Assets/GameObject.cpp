#include "Assets/GameObject.h"
#include "Core/Logger.h"

namespace Ethereal
{
	int32_t GameObject::s_NextID = 0;

	GameObject::GameObject()
	{
		m_Name = "GameObject" + std::to_string(m_ID);
		m_ID = s_NextID++;
		m_Model = std::make_shared<Model>();
		m_Model->TEMPSetup();
	}

	GameObject::GameObject(const std::string& name)
	{
		m_Name = name;	
		m_ID = s_NextID++;	
		m_Model = std::make_shared<Model>();
		m_Model->TEMPSetup();
	}

	GameObject::~GameObject()
	{

	}

	void GameObject::DrawUI(float deltaTime)
	{

	}

	void GameObject::Update(float deltaTime)
	{

	}

}