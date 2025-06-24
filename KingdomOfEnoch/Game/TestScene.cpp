#include "TestScene.h"
#include "TestObj.h"
#include "Core/Logger.h"
#include "Assets/GameObject.h"
#include "Core/EEContext.h"

TestScene::TestScene(const std::string& name)
	: Scene(name)
{
	auto assetManager = Ethereal::EEContext::Get().GetAssetManager();
	if (assetManager.LoadGameObject("TestCube"))
	{
		auto original = assetManager.Get<Ethereal::GameObject>("TestCube");
		for (int i = 0; i < 200; i++)
		{
			auto clone = original->Clone();
			AddGameObject(clone);
		}
	}
	else
	{
		LOG_WARN("Failed to load GameObject 'TestCube' in TestScene.");
	}
}

void TestScene::HandleInput(float deltaTime)
{
	
}

void TestScene::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}
