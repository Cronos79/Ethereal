#include "UI/EditorGui.h"
#include "Core/Logger.h"
#include "imgui/imgui_internal.h"
#include "Core/EngineUtils.h"
#include "Core/EEContext.h"

namespace Ethereal
{

	bool EditorGui::Initialize()
	{
		m_BaseAssetPath.clear();
		m_BaseAssetPath = GetAssetsDirectory();
		m_CurrentAssetPath.clear(); // Clears safely
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

		DrawMainMenuBar(deltaTime);
		DrawBottomBar(deltaTime, vpPos, vpSize, bottomBarHeight, rightBarWidth);
		DrawRightBar(deltaTime, vpPos, vpSize, bottomBarHeight, rightBarWidth);	
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

	void EditorGui::DrawBottomBar(float deltaTime, ImVec2 vpPos, ImVec2 vpSize, float bottomBarHeight, float rightBarWidth)
	{
		ImGui::SetNextWindowPos(ImVec2(vpPos.x, vpPos.y + vpSize.y - bottomBarHeight));
		ImGui::SetNextWindowSize(ImVec2(vpSize.x - rightBarWidth, bottomBarHeight));

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Asset Browser", nullptr, flags);

		if (m_CurrentAssetPath.empty())
		{
			return; // No valid path to display
		}
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
						m_SelectedAssetPath = path;
						ImGui::End();
						return; // Short-circuit to refresh new directory
					}
				}
				if (!entry.is_directory())
				{
					if (ImGui::Selectable(name.c_str(), m_SelectedAssetPath == path))
					{
						m_SelectedAssetPath = path;
						m_SelectedGameObject = nullptr; // Deselect scene object
					}
				}
			}
		}
		catch (std::exception& e)
		{
			ImGui::Text("Error reading folder: %s", e.what());
		}

		ImGui::End();
	}

	void EditorGui::DrawRightBar(float deltaTime, ImVec2 vpPos, ImVec2 vpSize, float bottomBarHeight, float rightBarWidth)
	{
		const float totalHeight = vpSize.y - m_MenuHeight;
		const float topHalfHeight = totalHeight * 0.5f;
		const float bottomHalfHeight = totalHeight - topHalfHeight - 4.0f;

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

		// === Top: Scene ===
		ImGui::SetNextWindowPos(ImVec2(vpPos.x + vpSize.x - rightBarWidth, vpPos.y + m_MenuHeight));
		ImGui::SetNextWindowSize(ImVec2(rightBarWidth, topHalfHeight));
		ImGui::Begin("Scene", nullptr, flags);

		std::vector<std::shared_ptr<Scene>> scenes = EEContext::Get().GetSceneManager().GetScenes();

		ImGui::Text("Scenes:");
		ImGui::BeginChild("SceneList", ImVec2(0, 100), true);

		for (int i = 0; i < scenes.size(); ++i)
		{
			const auto& scene = scenes[i];
			if (!scene) continue;

			bool selected = (m_CurrentSceneIndex == i);
			if (ImGui::Selectable(scene->GetName().c_str(), selected))
			{
				m_CurrentSceneIndex = i;

				// Load and get scene safely
				EEContext::Get().GetAssetManager().LoadScene(scene->GetName());
				auto scene1 = EEContext::Get().GetAssetManager().Get<Scene>(scene->GetName());

				if (scene1)
				{
					EEContext::Get().GetSceneManager().SetCurrentScene(scene1);
					LOG_INFO("Scene switched to: {}", scene1->GetName());
				}
				else
				{
					LOG_ERROR("Failed to load or retrieve scene '{}'", scene->GetName());
				}

				ImGui::EndChild();
				ImGui::End();
				return;
			}
		}
		ImGui::EndChild();

		// === GameObjects ===
		Scene* currentScene = EEContext::Get().GetSceneManager().GetCurrentScene();
		if (!currentScene)
		{
			ImGui::Text("No current scene set.");
			ImGui::End();
			return;
		}

		const std::string& sceneName = currentScene->GetName();
		ImGui::Text("Current Scene: %s", sceneName.c_str());

		auto& objs = currentScene->GetGameObjects();
		if (objs.empty())
		{
			ImGui::Text("Scene has no objects.");
		}
		else
		{
			ImGui::BeginChild("SceneScroll", ImVec2(0, 0), true);
			for (size_t i = 0; i < objs.size(); ++i)
			{
				const auto& obj = objs[i];
				if (!obj) continue;

				bool selected = (m_SelectedGameObject == obj);
				if (ImGui::Selectable(obj->GetName().c_str(), selected))
				{
					m_SelectedGameObject = obj;
				}
			}
			ImGui::EndChild();
		}

		ImGui::End();

		// === Bottom: Info / Inspector Tabs ===
		ImGui::SetNextWindowPos(ImVec2(vpPos.x + vpSize.x - rightBarWidth, vpPos.y + m_MenuHeight + topHalfHeight));
		ImGui::SetNextWindowSize(ImVec2(rightBarWidth, bottomHalfHeight));
		ImGui::Begin("InspectorPanel", nullptr, flags);

		if (ImGui::BeginTabBar("InspectorTabs"))
		{
			// Info Tab
			if (ImGui::BeginTabItem("Info"))
			{
				ImGuiIO& io = ImGui::GetIO();
				ImGui::Text("MousePos: %.1f, %.1f", io.MousePos.x, io.MousePos.y);
				ImGui::Text("DisplaySize: %.1f, %.1f", io.DisplaySize.x, io.DisplaySize.y);
				ImGui::Text("FramebufferScale: %.2f, %.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
				ImGui::Text("FPS: %.1f", io.Framerate);
				ImGui::Text("Frame Time: %.3f ms", 1000.0f / io.Framerate);
				ImGui::EndTabItem();
			}

			// Inspector Tab
			if (ImGui::BeginTabItem("Inspector"))
			{
				if (m_SelectedGameObject)
				{
					ImGui::Text("Selected GameObject:");
					ImGui::Text("Name: %s", m_SelectedGameObject->GetName().c_str());
				}
				else if (!m_SelectedAssetPath.empty())
				{
					ImGui::Text("Selected Asset:");
					ImGui::Text("File: %s", m_SelectedAssetPath.filename().string().c_str());
					ImGui::Text("Extension: %s", m_SelectedAssetPath.extension().string().c_str());
				}
				else
				{
					ImGui::Text("No object selected.");
				}
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}


