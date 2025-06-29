#pragma once

#include "EtherealIncludes.h"
#include "Assets/Scene.h"
#include <vector>
#include <string>
#include <memory>

namespace Ethereal
{
	class ETHEREAL_API SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void Initialize();

		// Access the current scene (non-owning pointer)
		Scene* GetCurrentScene() const;

		// Set the current scene
		bool SetCurrentScene(std::shared_ptr<Scene> scene);                    // From raw pointer
		bool SetCurrentScene(const std::string& sceneName);    // From name

		// Add a scene
		void AddScene(const std::shared_ptr<Scene>& scene, bool setCurrent = false);
		void RemoveScene(Scene* scene);                        // Can still remove by raw ptr
		void ClearScenes();

		// Get all scenes (shared_ptr list)
		const std::vector<std::shared_ptr<Scene>>& GetScenes() const;

	private:
		std::shared_ptr<Scene> FindScenePtr(Scene* scene) const;

	private:
		std::shared_ptr<Scene> m_CurrentScene;
		std::vector<std::shared_ptr<Scene>> m_Scenes;
	};
}