#include "pch.h"
#include "EEScene.h"
#include "Assets/ModelAsset.h"
#include "Renderer/DX12/D3D12Renderer.h"

namespace EtherealEngine
{
	EEScene::EEScene() = default;
	EEScene::~EEScene() = default;

	void EEScene::InitScene()
	{
		// Init m_GameObjects
		
		//m_GameObjects = 
	}

	void EEScene::AddGameObject(std::shared_ptr<EEGameObject> object)
	{
		m_GameObjects.push_back(object);
	}

	void EEScene::RemoveGameObject(std::shared_ptr<EEGameObject> object)
	{
		m_GameObjects.erase(
			std::remove(m_GameObjects.begin(), m_GameObjects.end(), object),
			m_GameObjects.end()
		);
	}

	void EEScene::Update(float deltaTime)
	{
		for (auto& obj : m_GameObjects)
		{
			obj->Update(deltaTime);
		}
	}

	void EEScene::Draw()
	{
		for (auto& obj : m_GameObjects)
		{
			auto model = obj->GetModel();
			if (model)
				EEContext::Get().GetRenderer()->Draw(model.get());
		}
	}

	void EEScene::DrawUI()
	{
		for (auto& obj : m_GameObjects)
		{
			obj->DrawUI();
		}
	}

	std::shared_ptr<EEGameObject> EEScene::FindByName(const std::string& name) const
	{
		for (const auto& obj : m_GameObjects)
		{
			if (obj->GetName() == name)
				return obj;
		}
		return nullptr;
	}
}