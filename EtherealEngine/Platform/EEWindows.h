#pragma once
#include "Core/EtherealIncludes.h"
#include <string>
#include "Platform/EEWinIncludes.h"

namespace Ethereal
{
	class ETHEREAL_API EEWindows
	{
	public:
		EEWindows();
		~EEWindows();
		EEWindows(const EEWindows&) = delete;
		EEWindows& operator=(const EEWindows&) = delete;

		bool Initialize();
		bool ProcessMessages();

		static LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		std::string m_windowTitle;
		std::string m_windowClassName = "EtherealEngineWindowClass";
	};
}
