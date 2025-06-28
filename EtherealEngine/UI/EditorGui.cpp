#include "UI/EditorGui.h"
#include "Core/Logger.h"
#include "imgui/imgui_internal.h"
#include "Core/EngineUtils.h"

namespace Ethereal
{

	bool EditorGui::Initialize()
	{
		m_BaseAssetPath = GetAssetsDirectory();
		m_CurrentAssetPath = m_BaseAssetPath;
		LOG_INFO("EditorGui Initialized");
		return true;
	}

	void EditorGui::DrawUI(float deltaTime)
	{
		const ImVec2 vpSize = ImGui::GetIO().DisplaySize;
		const ImVec2 vpPos = ImGui::GetMainViewport()->Pos;
		const float bottomBarHeight = 200.0f;
		const float rightBarWidth = 300.0f;

		m_Width = vpSize.x;
		m_Height = vpSize.y;
		m_vpPos = vpPos;

		DrawMainMenuBar(deltaTime);
		DrawAssetBrowser(deltaTime, vpPos, vpSize, bottomBarHeight, rightBarWidth);
		DrawInspector(deltaTime, vpPos, vpSize, bottomBarHeight, rightBarWidth);

		if (m_ShowFPS)
		{
			ImGui::Begin("Performance");
			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
			ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			ImGui::End();
		}
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

	void EditorGui::DrawAssetBrowser(float deltaTime, ImVec2 vpPos, ImVec2 vpSize, float bottomBarHeight, float rightBarWidth)
	{
		ImGui::SetNextWindowPos(ImVec2(vpPos.x, vpPos.y + vpSize.y - bottomBarHeight));
		ImGui::SetNextWindowSize(ImVec2(vpSize.x - rightBarWidth, bottomBarHeight));

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Asset Browser", nullptr, flags);

		// Show current path
		ImGui::Text("Path: %s", m_CurrentAssetPath.string().c_str());

		// Go back to parent folder
		if (m_CurrentAssetPath != m_BaseAssetPath)
		{
			if (ImGui::Button("<-- Back"))
			{
				m_CurrentAssetPath = m_CurrentAssetPath.parent_path();
				ImGui::End();
				return;
			}
		}

		// List directories and files
		try
		{
			for (const auto& entry : std::filesystem::directory_iterator(m_CurrentAssetPath))
			{
				const auto& path = entry.path();
				std::string name = path.filename().string();

				if (entry.is_directory())
				{
					if (ImGui::Selectable((name + "/").c_str()))
					{
						m_CurrentAssetPath = path;
						ImGui::End();
						return; // Short-circuit to refresh new directory
					}
				}
				else
				{
					ImGui::Text("%s", name.c_str());
				}
			}
		}
		catch (std::exception& e)
		{
			ImGui::Text("Error reading folder: %s", e.what());
		}

		ImGui::End();
	}

	void EditorGui::DrawInspector(float deltaTime, ImVec2 vpPos, ImVec2 vpSize, float bottomBarHeight, float rightBarWidth)
	{
		ImGui::SetNextWindowPos(ImVec2(vpPos.x + vpSize.x - rightBarWidth, vpPos.y + 17));
		ImGui::SetNextWindowSize(ImVec2(rightBarWidth, vpSize.y - 17));

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGuiIO& io = ImGui::GetIO();
		ImGui::Begin("Inspector", nullptr, flags);
		ImGui::Text("Inspector Placeholder");
		ImGui::Text("MousePos: %.1f, %.1f", io.MousePos.x, io.MousePos.y);
		ImGui::Text("DisplaySize: %.1f, %.1f", io.DisplaySize.x, io.DisplaySize.y);
		ImGui::Text("FramebufferScale: %.2f, %.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		ImGui::Text("FPS: %.1f", io.Framerate);
		ImGui::End();
	}

}


