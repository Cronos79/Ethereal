#pragma once

#include <memory>

#include "EELogger.h"
#include "ConfigManager.h"

namespace EtherealEngine
{
	// Main context class for Ethereal Engine
	class EEContext
	{
	public:
		static EEContext& Get()
		{
			static EEContext instance;
			return instance;
		}

		void Initialize();
		void Shutdown();

		// Subsystem accessors
		EELogger* GetLogger() const
		{
			return logger.get();
		}
		ConfigManager* GetConfigManager() const
		{
			return configManager.get();
		}
		//WindowSystem* GetWindowSystem() const
		//{
		//	return windowSystem.get();
		//}
		//AssetManager* GetAssetManager() const
		//{
		//	return assetManager.get();
		//}

		std::string GetConfigPath(const std::string& name);

		//configManager->Load(EEContext::Get().GetConfigPath("config.json"));

	private:
		EEContext() = default;
		~EEContext() = default;
		EEContext(const EEContext&) = delete;
		EEContext& operator=(const EEContext&) = delete;

		std::unique_ptr<EELogger> logger;
		std::unique_ptr<ConfigManager> configManager;
		//std::unique_ptr<WindowSystem> windowSystem;
		//std::unique_ptr<AssetManager> assetManager;
	};
}