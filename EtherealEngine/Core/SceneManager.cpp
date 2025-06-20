#include "Core/SceneManager.h"

namespace Ethereal
{
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{

	}

	void SceneManager::SetCurrentScene(Scene* scene)
	{
		if (scene == nullptr)
		{
			return; // Avoid setting null scenes
		}
		if (m_CurrentScene != nullptr)
		{
			m_CurrentScene->OnDeactivate();
		}
		m_CurrentScene = scene;
		m_CurrentScene->OnActivate();
	}

	void SceneManager::AddScene(Scene* scene, bool setCurrentScene)
	{
		if (scene == nullptr)
		{
			return; // Avoid adding null scenes
		}
		// Add the scene to m_Scenes
		m_Scenes.push_back(scene);
		if (m_Scenes.size() == 1 || setCurrentScene)
		{
			SetCurrentScene(scene);
		}
		else
		{
			scene->OnDeactivate(); // Deactivate the new scene if it's not the first one
		}
	}

	void SceneManager::RemoveScene(Scene* scene)
	{
		auto it = std::remove(m_Scenes.begin(), m_Scenes.end(), scene);
		if (it != m_Scenes.end())
		{
			m_Scenes.erase(it, m_Scenes.end());
		}
	}

	void SceneManager::ClearScenes()
	{
		for (Scene* scene : m_Scenes)
		{
			if (scene == m_CurrentScene)
			{
				m_CurrentScene = nullptr; // Clear current scene if it's being deleted
			}
			scene->OnDeactivate(); // Deactivate the scene before deletion
			delete scene;
		}
		m_Scenes.clear();
		m_CurrentScene = nullptr;
	}

	const std::vector<Ethereal::Scene*>& SceneManager::GetScenes()
	{
		return m_Scenes;
	}
}


