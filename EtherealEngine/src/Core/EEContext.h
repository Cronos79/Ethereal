#pragma once

#include <memory>
#include <stdint.h>

#include "Core/EELogger.h"
#include "Core/ConfigManager.h"

namespace EtherealEngine
{
	struct WindowSettings
	{
		int width = 1280;
		int height = 720;
		std::string title = "Unknown";
		bool fullscreen = false;
	};

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

		int32_t GetWindowWidth() const
		{
			return windowSettings.width;
		}
		int32_t GetWindowHeight() const
		{
			return windowSettings.height;
		}
		const std::string& GetWindowTitle() const
		{
			return windowSettings.title;
		}
		void SetWindowSize(int32_t width, int32_t height)
		{
			windowSettings.width = width;
			windowSettings.height = height;
		}
		void SetWindowTitle(const std::string& title)
		{
			windowSettings.title = title;
		}
		bool IsRunning() const
		{
			return isRunning;
		}
		void SetRunning(bool running)
		{
			isRunning = running;
		}

	private:
		EEContext() = default;
		~EEContext() = default;
		EEContext(const EEContext&) = delete;
		EEContext& operator=(const EEContext&) = delete;

		std::unique_ptr<EELogger> logger;
		std::unique_ptr<ConfigManager> configManager;
		//std::unique_ptr<WindowSystem> windowSystem;
		//std::unique_ptr<AssetManager> assetManager;

		WindowSettings windowSettings;
		bool isRunning = false;
	};
}