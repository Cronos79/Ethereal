#pragma once
#include "Core/EtherealIncludes.h"
#include <string>
#include "Platform/EEWinIncludes.h"
#include "Platform/Keyboard.h"
#include "Mouse.h"

namespace Ethereal
{
	class ETHEREAL_API EEWindows
	{
	public:
		EEWindows();
		~EEWindows();
		EEWindows(const EEWindows&) = delete;
		EEWindows& operator=(const EEWindows&) = delete;

		Keyboard& GetKeyboard()
		{
			return m_keyboard;
		}
		Mouse& GetMouse()
		{
			return m_Mouse;
		}

		bool Initialize();
		bool ProcessMessages();

		static LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		std::string m_windowTitle;
		std::string m_windowClassName = "EtherealEngineWindowClass";
		Keyboard m_keyboard;
		Mouse m_Mouse;
		HWND m_hWnd = nullptr;
	};
}
