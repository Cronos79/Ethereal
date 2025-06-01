#include "GameApp.h"
#include "Core/EELoggerMacros.h"
#include "imgui.h"



bool GameApp::Initialize()
{
	LOG_INFO("GameApp: Init");
	return true;
}

void GameApp::Update(float deltaTime)
{
	// Game logic here
	//LOG_INFO("Delta time: {:.3f} seconds", deltaTime);
	// 
	ImGui::ShowDemoWindow();
	// Make a imgui window showing the games fps
	ImGui::Begin("Game FPS");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

}

void GameApp::Shutdown()
{
	LOG_INFO("GameApp: Shutdown");
}