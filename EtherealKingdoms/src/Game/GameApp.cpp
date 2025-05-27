#include "GameApp.h"
#include "Core/EELoggerMacros.h"

bool GameApp::Initialize()
{
	LOG_INFO("GameApp: Init");
	return true;
}

void GameApp::Update(float deltaTime)
{
	// Game logic here
	LOG_INFO("Delta time: {:.3f} seconds", deltaTime);
}

void GameApp::Shutdown()
{
	LOG_INFO("GameApp: Shutdown");
}