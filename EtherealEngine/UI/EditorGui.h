#pragma once
#include "Core/EtherealIncludes.h"
#include "imgui/imgui.h"
#include <stdint.h>
#include <filesystem>

namespace Ethereal
{
	class ETHEREAL_API EditorGui
	{
	public:
		bool Initialize();
		void DrawUI(float deltaTime);
		void Shutdown();
	private:
		void DrawMainMenuBar(float deltaTime);
		void DrawAssetBrowser(float deltaTime, ImVec2 vpPos, ImVec2 vpSize, float bottomBarHeight, float rightBarWidth);
		void DrawInspector(float deltaTime, ImVec2 vpPos, ImVec2 vpSize, float bottomBarHeight, float rightBarWidth);

	private:
		bool m_ShowFPS = false;
		int32_t m_Width = 1;
		int32_t m_Height = 1;
		ImVec2 m_vpPos;
		std::filesystem::path m_BaseAssetPath;
		std::filesystem::path m_CurrentAssetPath;
	};
}