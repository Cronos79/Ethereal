#include "pch.h"
#include "EEContext.h"
//#include "WindowSystem.h"
//#include "AssetManager.h"

namespace EtherealEngine
{
	void EEContext::Initialize()
	{
		logger = std::make_unique<EELogger>();
		configManager = std::make_unique<ConfigManager>();
		//windowSystem = std::make_unique<WindowSystem>();
		//assetManager = std::make_unique<AssetManager>();
	}

	void EEContext::Shutdown()
	{
		//assetManager.reset();
		//windowSystem.reset();
		configManager.reset();
		logger.reset();
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

}