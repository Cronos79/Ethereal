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
	// Set Random rotation speed between .01 and .5 per second
	m_RotationSpeed = static_cast<float>((rand() % 50 + 1)) / 100.0f;
	m_RotationSpeed = m_RotationSpeed / 2.0f;
}

TestObj::~TestObj()
{

}

void TestObj::Update(float deltaTime)
{
	float speed = static_cast<float>(rand() % 140 + 10);
	speed = speed * deltaTime;	
	Move(deltaTime, speed, m_RotationSpeed);
	AddRotation(m_RndRotation.x * (m_RotationSpeed * deltaTime), m_RndRotation.y * (m_RotationSpeed * deltaTime), m_RndRotation.z * (m_RotationSpeed * deltaTime));
}

void TestObj::DrawUI(float deltaTime)
{
	GameObject::DrawUI(deltaTime);
}

std::shared_ptr<Ethereal::GameObject> TestObj::Clone() const
{
	auto clone = std::make_shared<TestObj>(); // creates TestObj not GameObject
	clone->SetName(m_Name);
	clone->SetTransform(m_Transform);
	clone->SetModel(m_Model);
	//clone->NewID();

	return clone;
}
