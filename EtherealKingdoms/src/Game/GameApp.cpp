#include "GameApp.h"
#include "Core/EELoggerMacros.h"
#include "imgui.h"
#include "Game/EEGameObject.h"
#include "Game/EEScene.h"

 //Test EEGameObject
class
	TestObject : public EtherealEngine::EEGameObject
{
	public:
	TestObject()
	{
		LOG_INFO("TestObject: Created");
	}
	~TestObject()
	{
		LOG_INFO("TestObject: Destroyed");
	}
	void Update(float deltaTime) override
	{
		ImGui::Begin("Game FPS");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::End();
	}
};


bool GameApp::Initialize()
{
	LOG_INFO("GameApp: Init");
	auto obj = std::make_shared<TestObject>();
	obj->SetName("TestObj");
	m_CurrentScene->AddGameObject(obj);
	return true;
}

void GameApp::Update(float deltaTime)
{
	
}

void GameApp::Shutdown()
{
	LOG_INFO("GameApp: Shutdown");
}