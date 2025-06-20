#pragma once
#include "EtherealIncludes.h"
#include "Assets/Scene.h"
#include <vector>
#include <string>


namespace Ethereal
{
	class ETHEREAL_API SceneManager
	{
	public:
		SceneManager();
		virtual ~SceneManager();
		void Initialize();

		Scene* GetCurrentScene() const
		{
			return m_CurrentScene;
		}
		void SetCurrentScene(Scene* scene);
		void AddScene(Scene* scene, bool setCurrentScene = false);
		void RemoveScene(Scene* scene);
		void ClearScenes();
		const std::vector<Scene*>& GetScenes();

	private:
		Scene* m_CurrentScene = nullptr;
		std::vector<Scene*> m_Scenes;
	};
}