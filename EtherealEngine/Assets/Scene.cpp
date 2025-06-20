#include "Assets/Scene.h"
#include "Core/EEContext.h"

namespace Ethereal
{

	Scene::Scene()
	{

	}

	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
	}

	Scene::~Scene()
	{
#ifdef EDITOR_BUILD
		if (m_Editor)
		{
			m_Editor->Shutdown();
			m_Editor.reset();
		}
#endif
	}

	void Scene::AddGameObject(GameObject obj)
	{
		m_GameObjects.push_back(obj);
	}

	void Scene::RemoveGameObject(GameObject obj)
	{
		auto it = std::remove(m_GameObjects.begin(), m_GameObjects.end(), obj);
		if (it != m_GameObjects.end())
		{
			m_GameObjects.erase(it, m_GameObjects.end());
		}
	}

	void Scene::RemoveGameObjectByName(const std::string& name)
	{
		auto it = std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
			[&name](const GameObject& obj) { return obj.GetName() == name; });
		if (it != m_GameObjects.end())
		{
			m_GameObjects.erase(it, m_GameObjects.end());
		}
	}

	void Scene::RemoveGameObjectByID(int32_t id)
	{
		auto it = std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
			[&id](const GameObject& obj) { return obj.GetID() == id; });
		if (it != m_GameObjects.end())
		{
			m_GameObjects.erase(it, m_GameObjects.end());
		}
	}

	void Scene::HandleInput(float deltaTime)
	{

	}

	void Scene::Update(float deltaTime)
	{
	}

	void Scene::DrawUI(float deltaTime)
	{
		for (auto& obj : m_GameObjects)
		{
			obj.DrawUI(deltaTime);
		}
	}

	void Scene::Render()
	{
		static bool firstRun = true;
		if (firstRun)
		{
#ifdef EDITOR_BUILD
			m_Editor = std::make_unique<EditorGui>();
			if (!m_Editor->Initialize())
			{
				LOG_ERROR("Failed to initialize editor GUI.");
				return;
			}
#endif
		}
		auto renderer = EEContext::Get().GetRenderer();
		renderer->BeginFrame();
		for (auto& obj : m_GameObjects)
		{
			if (obj.IsVisible())
			{
				renderer->Draw(obj);
			}
		}
#ifdef EDITOR_BUILD
		m_Editor->DrawUI(deltaTime);
#endif
		renderer->EndFrame();
	}
	const std::vector<GameObject>& Scene::GetGameObjects() const
	{
		return m_GameObjects;
	}

	void Scene::SetName(const std::string& name)
	{
		m_Name = name;
	}

	const std::string& Scene::GetName() const
	{
		return m_Name;
	}

	void Scene::OnActivate()
	{

	}

	void Scene::OnDeactivate()
	{
		
	}
}