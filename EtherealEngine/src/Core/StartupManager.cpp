#include "pch.h"
#include "StartupManager.h"
#include "EEContext.h"
#include "EELoggerMacros.h"
#include "ConfigManager.h"

namespace EtherealEngine
{

	bool StartupManager::Initialize()
	{
		EEContext::Get().Initialize();
		LOG_INFO("Ethereal Engine starting");		
		LoadWindowSettings();

		LOG_INFO("Ethereal Engine initialized successfully");
		EEContext::Get().SetRunning(true);
		return true;
	}

	void StartupManager::Shutdown()
	{

	}

	EtherealEngine::WindowSettings StartupManager::LoadWindowSettings()
	{
		auto cfg = EEContext::Get().GetConfigManager();
		// Get full path to the config file
		std::string path = EEContext::Get().GetConfigPath("Settings.json");
		cfg->Load(path);
		EtherealEngine::WindowSettings ws;
		ws.width = cfg->GetInt("window.width", 1280);
		ws.height = cfg->GetInt("window.height", 720);
		ws.title = cfg->GetString("window.title", "Unknown");
		ws.fullscreen = cfg->GetBool("window.fullscreen", false);

		EEContext::Get().SetWindowSize(ws.width, ws.height);
		EEContext::Get().SetWindowTitle(ws.title);

		LOG_INFO("Window: {} ({}x{})", ws.title, ws.width, ws.height);

		return ws;
	}

}