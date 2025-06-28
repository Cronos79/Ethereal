#pragma once
#include "Core/EtherealIncludes.h"
#include "UI/EditorGui.h"
#include <memory>
#include "SceneManager.h"

//#define EDITOR_BUILD

namespace Ethereal
{
	class ETHEREAL_API EEApplication
	{
	public:
		EEApplication();
		~EEApplication();
		void Initialize();
		void Run();
		void Shutdown();

		virtual void OnInitialize() = 0;
		virtual void OnHandleInput(float deltaTime) = 0;
		virtual void OnUpdate(float deltaTime) = 0;
		virtual void OnShutdown() = 0;

		// Get Current scenes Gameobjects getters
		std::vector<std::shared_ptr<GameObject>>& GetGameObjects()
		{
			return m_SceneManager.GetCurrentScene()->GetGameObjects();
		}
		std::shared_ptr<GameObject> GetGameObjectByName(const std::string& name)
		{
			return m_SceneManager.GetCurrentScene()->FindGameObjectByName(name);
		}
		std::shared_ptr<GameObject> GetGameObjectByID(const int32_t& Id)
		{
			return m_SceneManager.GetCurrentScene()->FindGameObjectByID(Id);
		}

	private:
		void InitializeNoesis();

	protected:
		SceneManager& m_SceneManager;
	};



}
