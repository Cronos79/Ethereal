#include "TestScene.h"
#include "TestObj.h"
#include "Core/Logger.h"

TestScene::TestScene(const std::string& name)
	: Scene(name)
{
	TestObj* obj = new TestObj();
	AddGameObject(*obj);
	LOG_INFO("Test scene loaded game obj {}", obj->GetName());
}

void TestScene::HandleInput(float deltaTime)
{
	
}

void TestScene::Update(float deltaTime)
{
	
}
