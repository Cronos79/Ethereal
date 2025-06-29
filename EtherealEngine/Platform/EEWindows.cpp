#include "Platform/EEWindows.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"
#include <windowsx.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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


		auto style = WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU; // Default window style
		RECT wr;
		Resize(EEContext::Get().GetWidth(), EEContext::Get().GetHeight(), wr, style);

		// Adjust window rect to account for the window title and borders
		int32_t windowWidth = EEContext::Get().GetWidth();
		int32_t windowHeight = EEContext::Get().GetHeight();
		m_windowTitle = EEContext::Get().GetWindowTitle();

		// Create a window
		m_hWnd = CreateWindowExA(
			0, // Optional window styles
			m_windowClassName.c_str(), // Window class name
			m_windowTitle.c_str(), // Window title
			style, // Window style
			wr.left, wr.top, windowWidth, windowHeight, // Size and position
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

		RECT clientRect;
		GetClientRect(m_hWnd, &clientRect);
		int clientWidth = clientRect.right - clientRect.left;
		int clientHeight = clientRect.bottom - clientRect.top;
		EEContext::Get().SetWidth(clientWidth);
		EEContext::Get().SetHeight(clientHeight);

		// Show the window
		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);

		EEContext::Get().SetWindowHandle(m_hWnd);

		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01; // Generic desktop controls
		rid.usUsage = 0x02;     // Mouse
		rid.dwFlags = 0;        // Or RIDEV_INPUTSINK if you want input even when not focused
		rid.hwndTarget = m_hWnd;
		if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
		{
			LOG_ERROR("Failed to register raw input device.");
		}

		m_Mouse.EnableRaw();// Enable raw mouse input

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

	void EEWindows::Resize(int width, int height, RECT& wr, long style)
	{
		// Make top and left center of the screen
	// Get the screen dimensions
		RECT desktopRect;
		GetClientRect(GetDesktopWindow(), &desktopRect);
		int screenWidth = desktopRect.right - desktopRect.left;
		int screenHeight = desktopRect.bottom - desktopRect.top;
		// Calculate the position to center the window
		int posX = (screenWidth - width) / 2; // Center horizontally
		int posY = (screenHeight - height) / 2; // Center vertically

		wr.left = posX; // Left position
		wr.top = posY; // Top position
		wr.right = wr.left + width; // Right position
		wr.bottom = wr.top + height; // Bottom position
		//= { 0, 0, windowWidth, windowHeight }; // Default size
		
		AdjustWindowRect(&wr, style, FALSE); // Adjust the rectangle for the window style

		width = wr.right - wr.left; // Adjusted width
		height = wr.bottom - wr.top; // Adjusted height

		EEContext::Get().SetHeight(height); // Set the adjusted size in the context
		EEContext::Get().SetWidth(width); // Set the adjusted size in the context
	}

	EEWindows::~EEWindows()
	{
		LOG_INFO("Windows platform destroyed.");
	}

	void EEWindows::HideCursor()
	{
		while (::ShowCursor(FALSE) >= 0);
	}

	void EEWindows::ConfineCursor()
	{
		RECT rect;
		GetClientRect(m_hWnd, &rect);
		MapWindowPoints(m_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}	

	// Windows procedure function to handle messages
	LRESULT CALLBACK EEWindows::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
			return true;

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

		int32_t width = EEContext::Get().GetWidth();
		int32_t height = EEContext::Get().GetHeight();

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

				// Fix for Shift, Ctrl, Alt (to detect left/right)
				if (key == VK_SHIFT)
				{
					key = static_cast<unsigned char>(
						MapVirtualKey((lParam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX));
				}
				else if (key == VK_CONTROL)
				{
					if (lParam & (1 << 24)) key = VK_RCONTROL;
					else key = VK_LCONTROL;
				}
				else if (key == VK_MENU) // Alt key
				{
					if (lParam & (1 << 24)) key = VK_RMENU;
					else key = VK_LMENU;
				}

				pEEWin->GetKeyboard().OnKeyDown(key);
			} break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			if (pEEWin)
			{
				unsigned char key = static_cast<unsigned char>(wParam);

				if (key == VK_SHIFT)
				{
					key = static_cast<unsigned char>(
						MapVirtualKey((lParam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX));
				}
				else if (key == VK_CONTROL)
				{
					if (lParam & (1 << 24)) key = VK_RCONTROL;
					else key = VK_LCONTROL;
				}
				else if (key == VK_MENU)
				{
					if (lParam & (1 << 24)) key = VK_RMENU;
					else key = VK_LMENU;
				}

				pEEWin->GetKeyboard().OnKeyUp(key);
			} break;
		case WM_CHAR:
			if (pEEWin)
			{
				unsigned int character = static_cast<unsigned int>(wParam);
				pEEWin->GetKeyboard().OnChar(character);
			} break;	
			/************* MOUSE MESSAGES ****************/
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			// cursorless exclusive gets first dibs
			if (!pEEWin->cursorEnabled)
			{
				if (!pEEWin->m_Mouse.IsInWindow())
				{
					SetCapture(hwnd);
					pEEWin->m_Mouse.OnMouseEnter();
					pEEWin->HideCursor();
				}
				break;
			}
			// stifle this mouse message if imgui wants to capture
			/*if (imio.WantCaptureMouse)
			{
				break;
			}*/
			// in client region -> log move, and log enter + capture mouse (if not previously in window)
			if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
			{
				pEEWin->m_Mouse.OnMouseMove(pt.x, pt.y);
				if (!pEEWin->m_Mouse.IsInWindow())
				{
					SetCapture(hwnd);
					pEEWin->m_Mouse.OnMouseEnter();
				}
			}
			// not in client -> log move / maintain capture if button down
			else
			{
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
				{
					pEEWin->m_Mouse.OnMouseMove(pt.x, pt.y);
				}
				// button up -> release capture / log event for leaving
				else
				{
					ReleaseCapture();
					pEEWin->m_Mouse.OnMouseLeave();
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			SetForegroundWindow(hwnd);
			if (!pEEWin->cursorEnabled)
			{
				pEEWin->ConfineCursor();
				pEEWin->HideCursor();
			}
			// stifle this mouse message if imgui wants to capture
			/*if (imio.WantCaptureMouse)
			{
				break;
			}*/
			const POINTS pt = MAKEPOINTS(lParam);
			pEEWin->m_Mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			// stifle this mouse message if imgui wants to capture
			/*if (imio.WantCaptureMouse)
			{
				break;
			}*/
			const POINTS pt = MAKEPOINTS(lParam);
			pEEWin->m_Mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			// stifle this mouse message if imgui wants to capture
			/*if (imio.WantCaptureMouse)
			{
				break;
			}*/
			const POINTS pt = MAKEPOINTS(lParam);
			pEEWin->m_Mouse.OnLeftReleased(pt.x, pt.y);
			// release mouse if outside of window
			if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
			{
				ReleaseCapture();
				pEEWin->m_Mouse.OnMouseLeave();
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			// stifle this mouse message if imgui wants to capture
			/*if (imio.WantCaptureMouse)
			{
				break;
			}*/
			const POINTS pt = MAKEPOINTS(lParam);
			pEEWin->m_Mouse.OnRightReleased(pt.x, pt.y);
			// release mouse if outside of window
			if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
			{
				ReleaseCapture();
				pEEWin->m_Mouse.OnMouseLeave();
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			// stifle this mouse message if imgui wants to capture
			/*if (imio.WantCaptureMouse)
			{
				break;
			}*/
			const POINTS pt = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			pEEWin->m_Mouse.OnWheelDelta(pt.x, pt.y, delta);
			break;
		}
		/************** END MOUSE MESSAGES **************/

		/************** RAW MOUSE MESSAGES **************/
		case WM_INPUT:
		{
			if (!pEEWin->m_Mouse.RawEnabled())
			{				
				break;
			}
			UINT size;
			// first get the size of the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER)) == -1)
			{
				// bail msg processing if error
				break;
			}
			pEEWin->rawBuffer.resize(size);
			// read in the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				pEEWin->rawBuffer.data(),
				&size,
				sizeof(RAWINPUTHEADER)) != size)
			{
				// bail msg processing if error
				break;
			}
			// process the raw input data
			auto& ri = reinterpret_cast<const RAWINPUT&>(*pEEWin->rawBuffer.data());
			if (ri.header.dwType == RIM_TYPEMOUSE &&
				(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			{
				pEEWin->m_Mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
			}
			break;
		}
		/************** END RAW MOUSE MESSAGES **************/
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam); // Default window procedure for unhandled messages
		}
	}
}