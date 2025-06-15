#pragma once
#include "Core/EEApplication.h"

class GameApp : public Ethereal::EEApplication
{
public:
	GameApp();
	~GameApp();

	void OnInitialize() override;
	void OnHandleInput() override;
	void OnUpdate() override;
	void OnShutdown() override;

private:

};
