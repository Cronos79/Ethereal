#include "TestScene.h"
#include "TestObj.h"
#include "Core/Logger.h"
#include "Assets/GameObject.h"
#include "Core/EEContext.h"

TestScene::TestScene(const std::string& name)
	: Scene(name)
{
	bool result = false;
	auto assetManager = Ethereal::EEContext::Get().GetAssetManager();
	result = assetManager.LoadGameObject("TestCube");
	if (!result)
	{
		LOG_WARN("Failed to load GameObject 'TestCube' in TestScene.");
	}
	result = assetManager.LoadGameObject("GO_SmallCone");
	if (!result)
	{
		LOG_WARN("Failed to load GameObject 'SmallCone' in TestScene.");
	}
	result = assetManager.LoadGameObject("GO_CubeRing");
	if (!result)
	{
		LOG_WARN("Failed to load GameObject 'CubeRing' in TestScene.");
	}
	result = assetManager.LoadGameObject("MainLight");
	if (!result)
	{
		LOG_WARN("Failed to load GameObject 'MainLight' in TestScene.");
	}

	if (result)
	{
		auto light = assetManager.Get<Ethereal::GameObject>("MainLight");
		auto lightclone = light->Clone();
		AddGameObject(lightclone);
		auto cubeRing = assetManager.Get<Ethereal::GameObject>("GO_CubeRing");
		auto cubeRingclone = cubeRing->Clone();
		AddGameObject(cubeRingclone);
	/*	auto originalCube = assetManager.Get<Ethereal::GameObject>("TestCube");
		auto originalCone = assetManager.Get<Ethereal::GameObject>("GO_SmallCone");*/
		for (int i = 0; i < 100; i++)
		{
		/*	auto clone = originalCube->Clone();
			AddGameObject(clone);
			auto coneClone = originalCone->Clone();
			AddGameObject(coneClone);*/
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

void TestScene::DrawUI(float deltaTime)
{
	Scene::DrawUI(deltaTime);

}
