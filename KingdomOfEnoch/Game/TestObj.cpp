#include "TestObj.h"
#include "Core/EEContext.h"
#include "Assets/Model.h"
#include "Core/Logger.h"

TestObj::TestObj()
	: GameObject("TestObj")
{
	Ethereal::EEContext::Get().GetAssetManager().LoadModel("TestModel");
	SetModel(Ethereal::EEContext::Get().GetAssetManager().Get<Ethereal::Model>("TestModel"));
	PickNewTarget();
	PickNewRandomRotation();
}

TestObj::~TestObj()
{

}

void TestObj::Update(float deltaTime)
{
	Move(deltaTime);
	AddRotation(m_RndRotation.x * (m_RotationSpeed * deltaTime), m_RndRotation.y * (m_RotationSpeed * deltaTime), m_RndRotation.z * (m_RotationSpeed * deltaTime));
}

void TestObj::DrawUI(float deltaTime)
{

}
