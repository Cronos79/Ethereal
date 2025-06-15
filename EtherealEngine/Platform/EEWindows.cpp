#include "Platform/EEWindows.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"


namespace Ethereal
{

	EEWindows::EEWindows()
	{
		LOG_INFO("Windows platform initialized.");
		m_windowTitle = EEContext::Get().GetWindowTitle();
	}

	bool EEWindows::Initialize()
	{
		// Perform initialization for the Windows platform here
		LOG_INFO("Windows platform is initializing.");
		// Windows class
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc; // Default window procedure
		wc.hInstance = GetModuleHandle(NULL);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszClassName = m_windowClassName.c_str();
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wc.lpszMenuName = NULL;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;

		if (!RegisterClassEx(&wc))
		{
			DWORD error = GetLastError();
			LOG_ERROR("Failed to register window class. Error code: {}", error);
			return false;
		}

		LOG_INFO("Window class registered successfully.");

		// Adjust window rect to account for the window title and borders
		int32_t windowWidth = EEContext::Get().GetWidth();
		int32_t windowHeight = EEContext::Get().GetHeight();
		RECT windowRect = { 0, 0, windowWidth, windowHeight }; // Default size
		auto style = WS_OVERLAPPEDWINDOW; // Default window style
		AdjustWindowRect(&windowRect, style, FALSE); // Adjust the rectangle for the window style

		// Create a window
		HWND hwnd = CreateWindowExA(
			0, // Optional window styles
			m_windowClassName.c_str(), // Window class name
			m_windowTitle.c_str(), // Window title
			style, // Window style
			CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, // Size and position
			NULL, // Parent window
			NULL, // Menu
			GetModuleHandle(NULL), // Instance handle
			this // Additional application data
		);

		if (!hwnd)
		{
			DWORD error = GetLastError();
			LOG_ERROR("Failed to create window. Error code: {}", error);
			return false; // Return false if window creation fails
		}
		LOG_INFO("Window created successfully.");

		// Show the window
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);

		EEContext::Get().SetWindowHandle(hwnd);

		return true; // Return true if initialization is successful
	}

	bool EEWindows::ProcessMessages()
	{
		// ' Process Windows messages in the message queue ' /
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				LOG_INFO("Received WM_QUIT message, exiting message loop.");
				return false; // Exit the message loop
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_DESTROY)
			{
				LOG_INFO("Received WM_DESTROY message, exiting message loop.");
				return false; // Exit the message loop on window destroy
			}
			if (msg.message == WM_CLOSE)
			{
				LOG_INFO("Received WM_CLOSE message, exiting message loop.");
				return false; // Exit the message loop on window close
			}
			if (msg.message == WM_QUIT)
			{
				LOG_INFO("Received WM_QUIT message, exiting message loop.");
				return false; // Exit the message loop on quit
			}
		}
		return true; // Continue processing messages
	}

	EEWindows::~EEWindows()
	{
		LOG_INFO("Windows platform destroyed.");
	}

	// Windows procedure function to handle messages
	LRESULT CALLBACK EEWindows::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
			LOG_INFO("WM_DESTROY received, posting quit message.");
			PostQuitMessage(0); // Post a quit message to exit the application
			return 0;
		case WM_CLOSE:
			LOG_INFO("WM_CLOSE received, posting quit message.");
			PostQuitMessage(0); // Post a quit message to exit the application
			return 0;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam); // Default window procedure for unhandled messages
		}
	}
}