#include "GameApp.h"
#include "Core/Logger.h"
#include "Core/EEContext.h"
#include "Platform/EEWindows.h"
#include "Platform/Keyboard.h"
#include "Platform/Mouse.h"
#include "Assets/Scene.h"
#include "Assets/GameObject.h"
#include "Game/TestObj.h"
#include "Game/TestScene.h"

GameApp::GameApp()
{

}

GameApp::~GameApp()
{

}

void GameApp::OnInitialize()
{
	// Temp code *************************
	TestScene* scene = new TestScene("TestScene");
	m_SceneManager.AddScene(scene);
	// Temp code *************************
	GAME_LOG_INFO("GameApp initialized successfully.");
}

void GameApp::OnHandleInput(float deltaTime)
{
	Ethereal::Keyboard& keyboard = Ethereal::EEContext::Get().GetWindow().GetKeyboard();
	Ethereal::Mouse& mouse = Ethereal::EEContext::Get().GetWindow().GetMouse();
	Ethereal::Camera& camera = Ethereal::EEContext::Get().GetCameraManager().GetCurrentCamera();	

	if (keyboard.GetKeyPressed(VK_ESCAPE))
	{
		LOG_INFO("Escape key pressed");
	}
	if (keyboard.GetKeyPressed(VK_F1))
	{
		GAME_LOG_INFO("F1 key pressed");
	}

	float newMouseSensitivity = m_MouseSensitivity;
	if (keyboard.IsKeyDown(VK_SHIFT))
	{
		newMouseSensitivity = m_MouseSensitivity * 2.0f; // Increase mouse sensitivity when Shift is pressed
	}
	camera.HandleInput(mouse, keyboard, 1.0f, m_CameraSpeed, newMouseSensitivity);
}

void GameApp::OnUpdate(float deltaTime)
{
	//LOG_INFO("GameApp is updating. Delta Time: {}", deltaTime);
}


void GameApp::OnShutdown()
{
	GAME_LOG_INFO("GameApp is shutting down.");
}
