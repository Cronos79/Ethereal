#include "Platform/EEWindows.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"
#include <windowsx.h>


namespace Ethereal
{

	EEWindows::EEWindows()
	{		
		m_windowTitle = EEContext::Get().GetWindowTitle();
	}

	bool EEWindows::Initialize()
	{
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

		// Adjust window rect to account for the window title and borders
		int32_t windowWidth = EEContext::Get().GetWidth();
		int32_t windowHeight = EEContext::Get().GetHeight();
		m_windowTitle = EEContext::Get().GetWindowTitle();

		RECT windowRect = { 0, 0, windowWidth, windowHeight }; // Default size
		auto style = WS_OVERLAPPEDWINDOW; // Default window style
		AdjustWindowRect(&windowRect, style, FALSE); // Adjust the rectangle for the window style

		// Create a window
		m_hWnd = CreateWindowExA(
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

		if (!m_hWnd)
		{
			DWORD error = GetLastError();
			LOG_ERROR("Failed to create window. Error code: {}", error);
			return false; // Return false if window creation fails
		}

		// Show the window
		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);

		EEContext::Get().SetWindowHandle(m_hWnd);

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
		EEWindows* pEEWin = nullptr;
		if (uMsg == WM_NCCREATE)
		{
			// Store the pointer to EEWindows in the window user data
			CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			pEEWin = static_cast<EEWindows*>(cs->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pEEWin));
		}
		else
		{
			// Retrieve the pointer to EEWindows
			pEEWin = reinterpret_cast<EEWindows*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

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
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if (pEEWin)
			{
				unsigned char key = static_cast<unsigned char>(wParam);
				pEEWin->GetKeyboard().OnKeyDown(key);
			} break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			if (pEEWin)
			{
				unsigned char key = static_cast<unsigned char>(wParam);
				pEEWin->GetKeyboard().OnKeyUp(key);
			} break;
		case WM_CHAR:
			if (pEEWin)
			{
				unsigned int character = static_cast<unsigned int>(wParam);
				pEEWin->GetKeyboard().OnChar(character);
			} break;
		case WM_MOUSEMOVE:
			if (pEEWin)
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				pEEWin->GetMouse().OnMove(x, y);
			} break;
		case WM_LBUTTONDOWN:
			if (pEEWin)
				pEEWin->GetMouse().OnButtonDown(Mouse::Left);
			break;
		case WM_LBUTTONUP:
			if (pEEWin)
				pEEWin->GetMouse().OnButtonUp(Mouse::Left);
			break;
		case WM_RBUTTONDOWN:
			if (pEEWin)
				pEEWin->GetMouse().OnButtonDown(Mouse::Right);
			break;
		case WM_RBUTTONUP:
			if (pEEWin)
				pEEWin->GetMouse().OnButtonUp(Mouse::Right);
			break;
		case WM_MBUTTONDOWN:
			if (pEEWin)
				pEEWin->GetMouse().OnButtonDown(Mouse::Middle);
			break;
		case WM_MBUTTONUP:
			if (pEEWin)
				pEEWin->GetMouse().OnButtonUp(Mouse::Middle);
			break;
		case WM_MOUSEWHEEL:
			if (pEEWin)
			{
				int delta = GET_WHEEL_DELTA_WPARAM(wParam);
				pEEWin->GetMouse().OnWheel(delta);
			} break;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam); // Default window procedure for unhandled messages
		}
	}
}