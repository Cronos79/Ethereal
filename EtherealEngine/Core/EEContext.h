#pragma once
// Project includes
#include "EtherealIncludes.h"
#include "Platform/EEWindows.h"
#include "Platform/EEWinIncludes.h"

// Standard library includes
#include <memory>
#include <string>


// Singleton for managing the context of the application
namespace Ethereal
{
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
		EEWindows& GetWindow() const
		{
			return *m_Window;
		} // Get the window object
		HWND GetWindowHandle() const
		{
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
			return m_windowTitle;
		}
		void SetWindowTitle(const std::string& title) // Set the title of the window
		{
			m_windowTitle = title;
		}
		int32_t GetWidth() const // Get the width of the window
		{
			return m_Width;
		}
		void SetWidth(int32_t width) // Set the width of the window
		{
			m_Width = width;
		}
		int32_t GetHeight() const // Get the height of the window
		{
			return m_Height;
		}
		void SetHeight(int32_t height) // Set the height of the window
		{
			m_Height = height;
		}
	private:
		std::unique_ptr<EEWindows> m_Window; // Pointer to the window object
		HWND m_hWnd; // Handle to the window
		bool m_IsRunning = true; // Flag to check if the application is running
		std::string m_windowTitle = "Default Title";
		int32_t m_Width = 1280; // Width of the window
		int32_t m_Height = 720; // Height of the window
	};
}
