#pragma once
#include "Assets/Scene.h"

class TestScene : public Ethereal::Scene
{
public:
	TestScene(const std::string& name);

	void HandleInput(float deltaTime) override;
	void Update(float deltaTime) override;
	void DrawUI(float deltaTime) override;

};
