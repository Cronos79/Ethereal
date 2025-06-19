#pragma once
// Project includes
#include "EtherealIncludes.h"
#include "Platform/EEWindows.h"
#include "Platform/EEWinIncludes.h"
#include "Core/AssetManager.h"

// Standard library includes
#include <memory>
#include <string>
#include "spdlog/fmt/bundled/format.h"
#include "Renderer/Renderer.h"

struct GameSettings
{
	std::string WindowTitle = "Default Title";
	int32_t Width = 1280; // Width of the window
	int32_t Height = 720; // Height of the window
	bool FullScreen = false;
};

// Singleton for managing the context of the application
namespace Ethereal
{
	class Renderer; // Forward declare your renderer 

	class ETHEREAL_API EEContext
	{
	public:
		// Get the singleton instance of EEContext
		static EEContext& Get()
		{
			static EEContext instance;
			return instance;
		}
		// Delete copy constructor and assignment operator to enforce singleton pattern
		EEContext(const EEContext&) = delete;
		EEContext& operator=(const EEContext&) = delete;
	private:
		// Private constructor to prevent instantiation from outside
		EEContext();
		// Destructor
		~EEContext();	

	public:
		void Initialize(); // Method to initialize the context

		// Assessors
		AssetManager& GetAssetManager() const // Get the asset manager
		{
			if (!m_AssetManager)
			{
				throw std::runtime_error("AssetManager is not initialized. Please initialize it first.");
			}
			return *m_AssetManager;
		} // Get the asset manager object
		EEWindows& GetWindow() const
		{
			return *m_Window;
		} // Get the window object
		HWND GetWindowHandle() const
		{
			if (!m_hWnd) // If the window handle is not set, retrieve it from the window object
			{
				throw std::runtime_error("Window handle is not set. Please initialize the window first.");
			}
			return m_hWnd;
		} // Get the handle to the window
		void SetWindowHandle(HWND hWnd) // Set the handle to the window
		{
			m_hWnd = hWnd;
		}
		bool IsRunning() const // Check if the application is running
		{
			return m_IsRunning;
		} // Get the running state of the application
		void SetRunning(bool isRunning) // Set the running state of the application
		{
			m_IsRunning = isRunning;
		}
		const std::string& GetWindowTitle() const // Get the title of the window
		{
			return m_GameSettings.WindowTitle;
		}
		void SetWindowTitle(const std::string& title) // Set the title of the window
		{
			m_GameSettings.WindowTitle = title;
		}
		int32_t GetWidth() const // Get the width of the window
		{
			return m_GameSettings.Width;
		}
		void SetWidth(int32_t width) // Set the width of the window
		{
			m_GameSettings.Width = width;
		}
		int32_t GetHeight() const // Get the height of the window
		{
			return m_GameSettings.Height;
		}
		void SetHeight(int32_t height) // Set the height of the window
		{
			m_GameSettings.Height = height;
		}
		bool GetFullScreen()
		{
			return m_GameSettings.FullScreen;
		}
		void SetFullScreen(bool fullScreen)
		{
			m_GameSettings.FullScreen = fullScreen;
		}
		// get the renderer
		std::shared_ptr<Renderer> GetRenderer() const
		{
			return m_Renderer;
		}
		// Get Device from renderer
		void* GetDevice() const
		{
			if (m_Renderer)
			{
				return m_Renderer->GetDevice();
			}
			throw std::runtime_error("Renderer is not initialized.");
		}

		void* GetContext() const
		{
			if (m_Renderer)
			{
				return m_Renderer->GetContext();
			}
			throw std::runtime_error("Renderer is not initialized.");
		}

	private:
		std::unique_ptr<EEWindows> m_Window; // Pointer to the window object
		std::unique_ptr<AssetManager> m_AssetManager;
		std::shared_ptr<Renderer> m_Renderer;

		HWND m_hWnd = nullptr; // Handle to the window
		bool m_IsRunning = true; // Flag to check if the application is running
		GameSettings m_GameSettings;
	};
}
