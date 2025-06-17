#include "GameApp.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"
#include "Platform/EEWindows.h"
#include "Platform/Keyboard.h"

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

void GameApp::OnHandleInput()
{
	Ethereal::Keyboard& keyboard = Ethereal::EEContext::Get().GetWindow().GetKeyboard();
	Ethereal::Mouse& mouse = Ethereal::EEContext::Get().GetWindow().GetMouse();
	if (keyboard.GetKeyPressed(VK_ESCAPE))
	{
		LOG_INFO("Escape key pressed");
	}
	else if (keyboard.GetKeyPressed(VK_F1))
	{
		GAME_LOG_INFO("F1 key pressed");
	}

	if (keyboard.IsKeyDown('W'))
	{
		GAME_LOG_INFO("W key is being held down");
	}

	if (mouse.GetButtonPressed(Ethereal::Mouse::Left))
	{
		LOG_INFO("Left mouse pressed");
	}
	if (mouse.IsButtonDown(Ethereal::Mouse::Right))
	{
		LOG_INFO("Right mouse button down");
	}
}

void GameApp::OnUpdate()
{
	
}

void GameApp::OnShutdown()
{
	GAME_LOG_INFO("GameApp is shutting down.");
}
