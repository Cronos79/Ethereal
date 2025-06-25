#include "Assets/Scene.h"
#include "Core/EEContext.h"
#include "Renderer/DX11/Camera.h"

namespace Ethereal
{

	Scene::Scene()
	{

	}

	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
	}

	Scene::~Scene()
	{
#ifdef EDITOR_BUILD
		if (m_Editor)
		{
			m_Editor->Shutdown();
			m_Editor.reset();
		}
#endif
	}

	void Scene::AddGameObject(const std::shared_ptr<GameObject>& obj)
	{
		m_GameObjects.push_back(obj);
	}

	void Scene::RemoveGameObject(const std::shared_ptr<GameObject>& obj)
	{
		auto it = std::remove(m_GameObjects.begin(), m_GameObjects.end(), obj);
		if (it != m_GameObjects.end())
		{
			m_GameObjects.erase(it, m_GameObjects.end());
		}
	}

	void Scene::RemoveGameObjectByName(const std::string& name)
	{
		auto it = std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
			[&name](const std::shared_ptr<GameObject>& obj) { return obj->GetName() == name; });
		if (it != m_GameObjects.end())
		{
			m_GameObjects.erase(it, m_GameObjects.end());
		}
	}

	void Scene::RemoveGameObjectByID(int32_t id)
	{
		auto it = std::remove_if(m_GameObjects.begin(), m_GameObjects.end(),
			[&id](const std::shared_ptr<GameObject>& obj) { return obj->GetID() == id; });
		if (it != m_GameObjects.end())
		{
			m_GameObjects.erase(it, m_GameObjects.end());
		}
	}

	std::shared_ptr<Ethereal::GameObject> Scene::FindGameObjectByName(const std::string& name)
	{
		auto it = std::find_if(m_GameObjects.begin(), m_GameObjects.end(),
			[&name](const std::shared_ptr<GameObject>& obj) { return obj->GetName() == name; });
		if (it != m_GameObjects.end())
		{
			return *it;
		}
		return nullptr;
	}

	std::shared_ptr<Ethereal::GameObject> Scene::FindGameObjectByID(int32_t id)
	{
		auto it = std::find_if(m_GameObjects.begin(), m_GameObjects.end(),
			[&id](const std::shared_ptr<GameObject>& obj) { return obj->GetID() == id; });
		if (it != m_GameObjects.end())
		{
			return *it;
		}
		return nullptr;
	}

	void Scene::HandleInput(float deltaTime)
	{

	}

	void Scene::Update(float deltaTime)
	{
		for (auto& obj : m_GameObjects)
		{
			obj->Update(deltaTime);
		}
	}

	void Scene::DrawUI(float deltaTime)
	{
		for (auto& obj : m_GameObjects)
		{
			obj->DrawUI(deltaTime);
		}
		Ethereal::Camera& camera = Ethereal::EEContext::Get().GetCameraManager().GetCurrentCamera();
		camera.UIControls(); // #TODO: Add bool to enable/disable camera UI controls

		ImGui::Begin("Performance");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::End();
	}

	void Scene::Render(float deltaTime)
	{
		static bool firstRun = true;
		if (firstRun)
		{
#ifdef EDITOR_BUILD
			m_Editor = std::make_unique<EditorGui>();
			if (!m_Editor->Initialize())
			{
				LOG_ERROR("Failed to initialize editor GUI.");
				return;
			}
#endif
			firstRun = false;
		}
		HandleInput(deltaTime);
		Update(deltaTime);
		auto renderer = EEContext::Get().GetRenderer();
		renderer->BeginFrame();
		for (auto& obj : m_GameObjects)
		{			
			if (obj->IsVisible())
			{				
				renderer->Draw(obj);			
			}
		}
		DrawUI(deltaTime);
#ifdef EDITOR_BUILD
		m_Editor->DrawUI(deltaTime);
#endif
		renderer->EndFrame();
	}

	std::shared_ptr<Ethereal::LightObject> Scene::GetMainLight()
	{
		if (m_CachedLight) return m_CachedLight;
		for (const auto& obj : m_GameObjects)
		{
			if (obj && obj->GetTypeName() == "LightObject")
			{
				m_CachedLight = std::dynamic_pointer_cast<LightObject>(obj);
				return m_CachedLight;
			}
		}
		return nullptr;
	}

	std::vector<std::shared_ptr<GameObject>>& Scene::GetGameObjects()
	{
		return m_GameObjects;
	}

	void Scene::SetName(const std::string& name)
	{
		m_Name = name;
	}

	const std::string& Scene::GetName() const
	{
		return m_Name;
	}

	void Scene::OnActivate()
	{

	}

	void Scene::OnDeactivate()
	{
		
	}
}