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
		void DrawBottomBar(float deltaTime, ImVec2 vpPos, ImVec2 vpSize, float bottomBarHeight, float rightBarWidth);
		void DrawRightBar(float deltaTime, ImVec2 vpPos, ImVec2 vpSize, float bottomBarHeight, float rightBarWidth);

	private:
		int32_t m_MenuHeight = 17; // Default height of the menu bar
		std::filesystem::path m_BaseAssetPath;
		std::filesystem::path m_CurrentAssetPath;
	};
}