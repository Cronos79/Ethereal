#include "UI/EditorGui.h"
#include "Core/Logger.h"
#include "imgui/imgui_internal.h"

namespace Ethereal
{

	bool EditorGui::Initialize()
	{
		LOG_INFO("EditorGui Initialized");
		return true;
	}

	void EditorGui::DrawUI(float deltaTime)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

		ImGui::Begin("DockSpace", nullptr, window_flags);

		DrawMainMenuBar(deltaTime);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		// Setup the layout only once
		SetupDockspaceLayout(dockspace_id);

		ImGui::End();
		ImGui::PopStyleVar(2);

		// Draw all docked windows every frame
		DrawViewport(deltaTime);
		DrawAssetBrowser(deltaTime);
		DrawInspector(deltaTime);
	}

	void EditorGui::Shutdown()
	{
		LOG_INFO("EditorGui Shutdown");
	}

	void EditorGui::DrawMainMenuBar(float deltaTime)
	{
		// Make a imgui menu bar at the top of the screen
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N")) {}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
				if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void EditorGui::SetupDockspaceLayout(ImGuiID dockspace_id)
	{
		static bool first_time = true;
		if (first_time)
		{
			first_time = false;

			ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

			// Only dock the viewport window in the center
			ImGui::DockBuilderDockWindow("Viewport", dockspace_id);

			ImGui::DockBuilderFinish(dockspace_id);
		}
	}

	void EditorGui::DrawViewport(float deltaTime)
	{
		
		ImGui::Begin("Viewport");	

		ImGui::End();
	}

	void EditorGui::DrawAssetBrowser(float deltaTime)
	{
		// Placeholder for asset browser UI
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Begin("Asset Browser");
		ImGui::Text("MousePos: %.1f, %.1f", io.MousePos.x, io.MousePos.y);
		ImGui::Text("DisplaySize: %.1f, %.1f", io.DisplaySize.x, io.DisplaySize.y);
		ImGui::Text("FramebufferScale: %.2f, %.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		ImGui::Text("FPS: %.1f", io.Framerate);
		ImGui::End();
	}


	void EditorGui::DrawInspector(float deltaTime)
	{
		ImGui::Begin("Inspector");
		ImGui::Text("Inspector Placeholder");
		ImGui::End();
	}

}