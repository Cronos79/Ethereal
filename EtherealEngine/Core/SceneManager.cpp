#include "Core/SceneManager.h"
#include "Core/EEContext.h"

namespace Ethereal
{
	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
		ClearScenes();
	}

	void SceneManager::Initialize()
	{
		// Optional: preload or load scenes here if needed
	}

	Scene* SceneManager::GetCurrentScene() const
	{
		return m_CurrentScene.get(); // Return raw pointer for convenience
	}

	bool SceneManager::SetCurrentScene(std::shared_ptr<Scene> scene)
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
		// If m_scenes already has scene dont add 
		if (std::find(m_Scenes.begin(), m_Scenes.end(), scene) == m_Scenes.end())
		{
			m_Scenes.push_back(scene);
		}
		return true;
	}

	bool SceneManager::SetCurrentScene(const std::string& sceneName)
	{
		auto scene = EEContext::Get().GetAssetManager().Get<Scene>(sceneName);
		if (scene)
		{
			return SetCurrentScene(scene);
		}
		return false;
	}

	void SceneManager::AddScene(const std::shared_ptr<Scene>& scene, bool setCurrent)
	{
		if (!scene)
			return;

		// Prevent duplicates
		if (std::find(m_Scenes.begin(), m_Scenes.end(), scene) == m_Scenes.end())
		{
			m_Scenes.push_back(scene);
		}

		if (m_Scenes.size() == 1 || setCurrent)
		{
			if (m_CurrentScene)
				m_CurrentScene->OnDeactivate();

			m_CurrentScene = scene;
			m_CurrentScene->OnActivate();
		}
		else
		{
			scene->OnDeactivate();
		}
	}

	void SceneManager::RemoveScene(Scene* scene)
	{
		auto it = std::remove_if(m_Scenes.begin(), m_Scenes.end(),
			[scene](const std::shared_ptr<Scene>& s) { return s.get() == scene; });

		if (it != m_Scenes.end())
		{
			// If it's the current scene, clear it
			if (m_CurrentScene && m_CurrentScene.get() == scene)
				m_CurrentScene = nullptr;

			m_Scenes.erase(it, m_Scenes.end());
		}
	}

	void SceneManager::ClearScenes()
	{
		if (m_CurrentScene)
		{
			m_CurrentScene->OnDeactivate();
			m_CurrentScene = nullptr;
		}

		for (auto& scene : m_Scenes)
		{
			if (scene)
				scene->OnDeactivate();
		}

		m_Scenes.clear(); // shared_ptr handles destruction
	}

	const std::vector<std::shared_ptr<Scene>>& SceneManager::GetScenes() const
	{
		return m_Scenes;
	}

	std::shared_ptr<Scene> SceneManager::FindScenePtr(Scene* rawScene) const
	{
		for (const auto& scene : m_Scenes)
		{
			if (scene.get() == rawScene)
				return scene;
		}
		return nullptr;
	}
}