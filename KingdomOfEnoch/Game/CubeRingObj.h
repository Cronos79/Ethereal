#pragma once
#include "Assets/GameObject.h"

class CubeRingObj : public Ethereal::GameObject
{
public:
	CubeRingObj();
	virtual ~CubeRingObj() = default;

	virtual void Update(float deltaTime) override;
	virtual void DrawUI(float deltaTime) override;
};