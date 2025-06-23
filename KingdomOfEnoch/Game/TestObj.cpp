#include "TestObj.h"
#include "Core/EEContext.h"
#include "Assets/Model.h"
#include "Core/Logger.h"
#include "Core/EngineUtils.h"

REGISTER_GAMEOBJECT_TYPE(TestObj);

TestObj::TestObj()
	: GameObject("TestObj")
{
	SetTypeName("TestObj"); // important for clone consistency

	PickNewTarget();
	PickNewRandomRotation();

	// Random rotation speed
	m_RotationSpeed = RandomFloatRange(0.005f, 0.25f);
}

void TestObj::Update(float deltaTime)
{
	float speed = RandomFloatRange(10.0f, 150.0f) * deltaTime;
	Move(deltaTime, speed, m_RotationSpeed);

	AddRotation(
		m_RndRotation.x * (m_RotationSpeed * deltaTime),
		m_RndRotation.y * (m_RotationSpeed * deltaTime),
		m_RndRotation.z * (m_RotationSpeed * deltaTime)
	);
}

void TestObj::DrawUI(float deltaTime)
{
	GameObject::DrawUI(deltaTime);
}
