#pragma once

#include <memory>
#include <stdint.h>

#include "Core/EELogger.h"
#include "Core/ConfigManager.h"
#include "Assets/AssetManager.h"
#include "Platform/EEWindow.h"

namespace EtherealEngine
{
	struct WindowSettings
	{
		int width = 1280;
		int height = 720;
		std::wstring title = L"Unknown";
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
		void InitializeWin();
		void Shutdown();
		void ProcessEvents();

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
		AssetManager* GetAssetManager() const
		{
			return assetManager.get();
		}

		std::string GetConfigPath(const std::string& name);
		std::string GetAssetPath(const std::string& relative);

		int32_t GetWindowWidth() const
		{
			return windowSettings.width;
		}
		int32_t GetWindowHeight() const
		{
			return windowSettings.height;
		}
		const std::wstring& GetWindowTitleW() const
		{
			return windowSettings.title;
		}
		const std::string GetWindowTitle() const
		{
			return std::string(windowSettings.title.begin(), windowSettings.title.end());
		}
		void SetWindowSize(int32_t width, int32_t height)
		{
			windowSettings.width = width;
			windowSettings.height = height;
		}
		void SetWindowTitle(const std::wstring& title)
		{
			windowSettings.title = title;
		}
		void SetWindowTitle(const std::string& title)
		{
			windowSettings.title = std::wstring(title.begin(), title.end());
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
		std::unique_ptr<EEWindow> m_Window;
		std::unique_ptr<AssetManager> assetManager;

		WindowSettings windowSettings;
		bool isRunning = false;
	};
}