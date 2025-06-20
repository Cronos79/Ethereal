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

	protected:
		SceneManager& m_SceneManager;
	};



}
