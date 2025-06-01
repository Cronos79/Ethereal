#pragma once

#include <memory>
#include <stdint.h>

#include "Core/EELogger.h"
#include "Core/ConfigManager.h"
#include "Assets/AssetManager.h"
#include "Platform/EEWindow.h"
#include <Renderer/DX12/D3D12Renderer.h>

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

		std::string GetConfigPath(const std::string& name);
		std::string GetAssetPath(const std::string& relative);

		// Subsystem accessors
		EELogger* GetLogger() const
		{
			return m_Logger.get();
		}
		ConfigManager* GetConfigManager() const
		{
			return m_ConfigManager.get();
		}
		AssetManager* GetAssetManager() const
		{
			return m_AssetManager.get();
		}	
		int32_t GetWindowWidth() const
		{
			return m_WindowSettings.width;
		}
		int32_t GetWindowHeight() const
		{
			return m_WindowSettings.height;
		}
		const std::wstring& GetWindowTitleW() const
		{
			return m_WindowSettings.title;
		}
		const std::string GetWindowTitle() const
		{
			return std::string(m_WindowSettings.title.begin(), m_WindowSettings.title.end());
		}
		const HWND GetWindowHandle() const
		{
			return m_Window ? m_Window->GetHWND() : nullptr;
		}
		EEWindow* GetWindow() const
		{
			return m_Window.get();
		}
		D3D12Renderer* GetRenderer() const
		{
			return m_Renderer.get();
		}
		void SetWindowSize(int32_t width, int32_t height)
		{
			m_WindowSettings.width = width;
			m_WindowSettings.height = height;
		}
		void SetWindowTitle(const std::wstring& title)
		{
			m_WindowSettings.title = title;
		}
		void SetWindowTitle(const std::string& title)
		{
			m_WindowSettings.title = std::wstring(title.begin(), title.end());
		}
		bool IsRunning() const
		{
			return m_isRunning;
		}
		void SetRunning(bool running)
		{
			m_isRunning = running;
		}

		void InitLogger();
		void InitConfigManager();
		void InitAssetManager();
		void InitWindows();
		void InitRenderer();
		void Shutdown();
		void ProcessEvents();

	private:
		EEContext() = default;
		~EEContext() = default;
		EEContext(const EEContext&) = delete;
		EEContext& operator=(const EEContext&) = delete;

		std::unique_ptr<EELogger> m_Logger;
		std::unique_ptr<ConfigManager> m_ConfigManager;
		//std::unique_ptr<WindowSystem> windowSystem;
		std::unique_ptr<EEWindow> m_Window;
		std::unique_ptr<AssetManager> m_AssetManager;
		std::unique_ptr<D3D12Renderer> m_Renderer; 

		WindowSettings m_WindowSettings;
		bool m_isRunning = false;
	};
}