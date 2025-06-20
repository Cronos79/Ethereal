#pragma once
#include "Core/EtherealIncludes.h"
#include "imgui/imgui.h"

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
		void SetupDockspaceLayout(ImGuiID dockspace_id);
		void DrawViewport(float deltaTime);
		void DrawAssetBrowser(float deltaTime);
		void DrawInspector(float deltaTime);
	};
}