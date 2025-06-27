#include "Core/SceneManager.h"
#include "EEContext.h"

namespace Ethereal
{
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{

	}

	bool SceneManager::SetCurrentScene(Scene* scene)
	{
		if (scene == nullptr)
		{
			return false; // Avoid setting null scenes
		}
		if (m_CurrentScene != nullptr)
		{
			m_CurrentScene->OnDeactivate();
		}
		m_CurrentScene = scene;
		m_CurrentScene->OnActivate();
		return true;
	}

	bool SceneManager::SetCurrentScene(const std::string& sceneName)
	{
		auto scene = EEContext::Get().GetAssetManager().Get<Scene>(sceneName);
		if (scene)
		{
			return SetCurrentScene(scene.get());
		}
		return false;
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


