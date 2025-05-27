#pragma once
#include "Core/Application.h"

class GameApp : public EtherealEngine::Application
{
public:
	virtual bool Initialize() override;
	virtual void Update(float deltaTime) override;
	virtual void Shutdown() override;
};