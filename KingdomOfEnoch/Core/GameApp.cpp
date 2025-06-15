#include "GameApp.h"
#include "Core/Logger.h"

GameApp::GameApp()
{

}

GameApp::~GameApp()
{

}

void GameApp::OnInitialize()
{
	GAME_LOG_INFO("GameApp initialized successfully.");
}

void GameApp::OnUpdate()
{
	
}

void GameApp::OnShutdown()
{
	GAME_LOG_INFO("GameApp is shutting down.");
}
