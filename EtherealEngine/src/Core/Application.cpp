#include "pch.h"
#include "Core/Application.h"
#include "Core/EEContext.h"
#include "Core/StartupManager.h"
#include <chrono>
#include <thread>

namespace EtherealEngine
{

	int Application::Run()
	{
		StartupManager startup;
		if (!startup.Initialize())
		{
			LOG_ERROR("Failed to initialize engine.");
			return -1;
		}

		m_CurrentScene = std::make_unique<EEScene>();
		if (!Initialize())
		{
			LOG_ERROR("Game-specific initialization failed.");
			return -2;
		}

		auto lastTime = std::chrono::high_resolution_clock::now();
		
		while (EEContext::Get().IsRunning())
		{
			EEContext::Get().ProcessEvents();
			auto now = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float>(now - lastTime).count();
			lastTime = now;
			
			EEContext::Get().GetRenderer()->BeginFrame();
			
			if (m_CurrentScene)
			{
				Update(deltaTime);
				m_CurrentScene->Update(deltaTime);
				m_CurrentScene->Draw();
				m_CurrentScene->DrawUI();
			}
			EEContext::Get().GetRenderer()->EndFrame();
			EEContext::Get().GetRenderer()->PresentFrame();
		}

		Shutdown();
		startup.Shutdown();
		EEContext::Get().Shutdown();
		return 0;
	}

	bool Application::Initialize()
	{
		LOG_INFO("Base Application initialized.");
		return true;
	}

	void Application::Update(float deltaTime)
	{
		// Default does nothing
	}

	void Application::Shutdown()
	{
		m_CurrentScene.reset();
		LOG_INFO("Base Application shutdown.");		
	}

} // namespace EtherealEngine