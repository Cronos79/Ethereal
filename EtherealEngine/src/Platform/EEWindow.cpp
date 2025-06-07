#include "pch.h"
#include "EEWindow.h"
#include "./Core/EELoggerMacros.h"
#include "Core/EEContext.h"
#include <imgui_impl_win32.h>
#include <windowsx.h>

using namespace EtherealEngine;

EEWindow::EEWindow() = default;
EEWindow::~EEWindow() = default;

bool EEWindow::Initialize(const std::wstring& title, int width, int height)
{
	m_Title = title;
	m_Width = width;
	m_Height = height;
	m_HInstance = GetModuleHandle(nullptr);

	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_HInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"EtherealWindowClass";

	if (!RegisterClassExW(&wc))
	{
		LOG_ERROR("Failed to register window class.");
		return false;
	}

	RECT wr = { 0, 0, width, height };
	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	AdjustWindowRect(&wr, style, FALSE);

	m_Hwnd = CreateWindowExW(
		0,
		wc.lpszClassName,
		m_Title.c_str(),
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		m_HInstance,
		this // Pass `this` pointer
	);

	if (!m_Hwnd)
	{
		LOG_ERROR("Failed to create window.");
		return false;
	}

	EEContext::Get().SetRunning(true);
	ShowWindow(m_Hwnd, SW_SHOW);
	UpdateWindow(m_Hwnd);

	return true;
}

void EEWindow::PollEvents()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			EEContext::Get().SetRunning(false);
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EEWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

	EEWindow* window = nullptr;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		window = static_cast<EEWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
	}
	else
	{
		window = reinterpret_cast<EEWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	if (window)
	{
		switch (uMsg)
		{
		case WM_CLOSE:
			EEContext::Get().SetRunning(false);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if (!(lParam & 0x40000000)) // Only on initial press, not repeat
				window->m_Keyboard.OnKeyDown(static_cast<uint8_t>(wParam));
			break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			window->m_Keyboard.OnKeyUp(static_cast<uint8_t>(wParam));
			break;
		case WM_LBUTTONDOWN:
			window->m_Mouse.OnButtonDown(Mouse::Left);
			break;
		case WM_LBUTTONUP:
			window->m_Mouse.OnButtonUp(Mouse::Left);
			break;
		case WM_RBUTTONDOWN:
			window->m_Mouse.OnButtonDown(Mouse::Right);
			break;
		case WM_RBUTTONUP:
			window->m_Mouse.OnButtonUp(Mouse::Right);
			break;
		case WM_MBUTTONDOWN:
			window->m_Mouse.OnButtonDown(Mouse::Middle);
			break;
		case WM_MBUTTONUP:
			window->m_Mouse.OnButtonUp(Mouse::Middle);
			break;
		case WM_MOUSEMOVE:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			window->m_Mouse.OnMove(x, y);
			break;
		}
		}

	}

	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}