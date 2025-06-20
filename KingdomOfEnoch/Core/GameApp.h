#pragma once
#include "Core/EEApplication.h"

class GameApp : public Ethereal::EEApplication
{
public:
	GameApp();
	~GameApp();

	void OnInitialize() override;
	void OnHandleInput(float deltaTime) override;
	void OnUpdate(float deltaTime) override;
	void OnShutdown() override;

private:
	float m_CameraSpeed = 0.5f;
	float m_MouseSensitivity = 0.0002f; // Adjust as needed
};
