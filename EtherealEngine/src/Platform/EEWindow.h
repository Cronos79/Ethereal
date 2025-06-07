#pragma once
#include "WinInclude.h" // Include the necessary Windows headers
#include <string>
#include "Keyboard.h"
#include "Mouse.h"

namespace EtherealEngine
{
	class EEWindow
	{
	public:
		EEWindow();
		~EEWindow();

		bool Initialize(const std::wstring& title, int width, int height);
		void PollEvents(); // Non-blocking message loop

		HWND GetHWND() const
		{
			return m_Hwnd;
		}
		int GetWidth() const
		{
			return m_Width;
		}
		int GetHeight() const
		{
			return m_Height;
		}
		Keyboard& GetKeyboard()
		{
			return m_Keyboard;
		}
		Mouse& GetMouse()
		{
			return m_Mouse;
		}

	private:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		HWND m_Hwnd = nullptr;
		HINSTANCE m_HInstance = nullptr;
		std::wstring m_Title;
		int m_Width = 1280;
		int m_Height = 720;
		Keyboard m_Keyboard;
		Mouse m_Mouse;
	};
}