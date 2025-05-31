#include "pch.h"
#include "EEContext.h"

namespace EtherealEngine
{
	void EEContext::Initialize()
	{
		logger = std::make_unique<EELogger>();
		configManager = std::make_unique<ConfigManager>();
		assetManager = std::make_unique<AssetManager>();
		assetManager->Initialize();		
	}

	void EEContext::InitializeWin()
	{
		m_Window = std::make_unique<EEWindow>();
		m_Window->Initialize(windowSettings.title, windowSettings.width, windowSettings.height);
	}

	void EEContext::Shutdown()
	{
		SetRunning(false);
		assetManager.reset();
		//windowSystem.reset();
		m_Window.reset();
		configManager.reset();
		logger.reset();
	}

	void EEContext::ProcessEvents()
	{
		if (m_Window)
		{
			m_Window->PollEvents();
		}
	}

	std::filesystem::path GetExecutableDir()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(nullptr, buffer, MAX_PATH);
		std::filesystem::path exePath(buffer);
		return exePath.parent_path();
	}

	std::string EEContext::GetConfigPath(const std::string& name)
	{
		auto configPath = GetExecutableDir() / "config" / name;
		return configPath.string();
	}

	std::string EEContext::GetAssetPath(const std::string& relative)
	{
		auto configPath = GetExecutableDir() / "Assets" / relative;
		return configPath.string();
	}

}