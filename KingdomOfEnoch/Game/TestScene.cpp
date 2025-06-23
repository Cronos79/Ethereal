#include "TestScene.h"
#include "TestObj.h"
#include "Core/Logger.h"
#include "Assets/GameObject.h"
#include "Core/EEContext.h"

TestScene::TestScene(const std::string& name)
	: Scene(name)
{
	//for (int i = 0; i < 200; i++)
	//{
		//auto obj = std::make_shared<TestObj>();
		//AddGameObject(obj);
		//LOG_INFO("Test scene loaded game obj {}", obj->GetName());
	//}
	auto assetManager = Ethereal::EEContext::Get().GetAssetManager();
	assetManager.LoadGameObject("TestCube");
	auto original = assetManager.Get<Ethereal::GameObject>("TestCube");

	for (int i = 0; i < 200; i++)
	{
		auto clone = original->Clone();
		AddGameObject(clone);

		LOG_INFO("Test scene loaded game obj {}", clone->GetName());
	}
}

void TestScene::HandleInput(float deltaTime)
{
	
}

void TestScene::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}
