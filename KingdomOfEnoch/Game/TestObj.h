#pragma once
#include "Assets/GameObject.h"

class TestObj : public Ethereal::GameObject
{
public:
	TestObj();
	~TestObj();

	virtual void Update(float deltaTime) override;
	virtual void DrawUI(float deltaTime) override;
};