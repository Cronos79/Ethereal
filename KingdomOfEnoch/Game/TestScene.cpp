#include "TestScene.h"
#include "TestObj.h"
#include "Core/Logger.h"

TestScene::TestScene(const std::string& name)
	: Scene(name)
{
	for (int i = 0; i < 200; i++)
	{
		auto obj = std::make_shared<TestObj>();
		AddGameObject(obj);
		LOG_INFO("Test scene loaded game obj {}", obj->GetName());
	}
}

void TestScene::HandleInput(float deltaTime)
{
	
}

void TestScene::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}
