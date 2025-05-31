#include "pch.h"
#include "EEContext.h"

namespace EtherealEngine
{  
	void EEContext::InitLogger()
	{
		m_Logger = std::make_unique<EELogger>();
	}

	void EEContext::InitConfigManager()
	{
		m_ConfigManager = std::make_unique<ConfigManager>();
	}

	void EEContext::InitAssetManager()
	{
		m_AssetManager = std::make_unique<AssetManager>();
		m_AssetManager->Initialize();
	}

	void EEContext::InitWindows()
	{
		m_Window = std::make_unique<EEWindow>();
		m_Window->Initialize(m_WindowSettings.title, m_WindowSettings.width, m_WindowSettings.height);
	}

	void EEContext::Shutdown()
	{
		SetRunning(false);
		m_AssetManager.reset();
		m_Window.reset();
		m_ConfigManager.reset();
		m_Logger.reset();
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