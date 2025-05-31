#include "pch.h"
#include "StartupManager.h"
#include "EEContext.h"
#include "EELoggerMacros.h"
#include "ConfigManager.h"
#include <locale>
#include <codecvt>

namespace EtherealEngine
{
	// Helper function for conversion (C++17 and earlier)
	inline std::wstring StringToWString(const std::string& str)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(str);
	}

	inline std::string WStringToString(const std::wstring& wstr)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(wstr);
	}

	bool StartupManager::Initialize()
	{
		EEContext::Get().Initialize();
		LOG_INFO("Ethereal Engine starting");
		LoadWindowSettings();
		EEContext::Get().InitializeWin();

		LOG_INFO("Ethereal Engine initialized successfully");
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
		ws.title = StringToWString(cfg->GetString("window.title", "Unknown"));
		ws.fullscreen = cfg->GetBool("window.fullscreen", false);

		EEContext::Get().SetWindowSize(ws.width, ws.height);
		EEContext::Get().SetWindowTitle(ws.title);

		LOG_INFO("Window: {} ({}x{})", WStringToString(ws.title), ws.width, ws.height);

		return ws;
	}

}