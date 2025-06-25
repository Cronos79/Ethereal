#pragma once
#include "Core/EtherealIncludes.h"
#include <vector>
#include "Assets/GameObject.h"
#include <stdint.h>
#include <memory>
#include "UI/EditorGui.h"
#include "LightObject.h"

namespace Ethereal
{
	class ETHEREAL_API Scene
	{
	public:
		Scene();
		Scene(const std::string& name);
		virtual ~Scene();
		void AddGameObject(const std::shared_ptr<GameObject>& obj);
		void RemoveGameObject(const std::shared_ptr<GameObject>& obj);
		void RemoveGameObjectByName(const std::string& name);
		void RemoveGameObjectByID(int32_t id);
		std::shared_ptr<GameObject> FindGameObjectByName(const std::string& name);
		std::shared_ptr<GameObject> FindGameObjectByID(int32_t id);
		virtual void HandleInput(float deltaTime);
		virtual void Update(float deltaTime);
		virtual void DrawUI(float deltaTime);
		void Render(float deltaTime);

		std::shared_ptr<LightObject> GetMainLight();

		std::vector<std::shared_ptr<GameObject>>& GetGameObjects();
		void SetName(const std::string& name);
		const std::string& GetName() const;

		void OnActivate();
		void OnDeactivate();	
	private:

	private:
		std::vector<std::shared_ptr<GameObject>> m_GameObjects;
		std::string m_Name;
		std::unique_ptr<EditorGui> m_Editor;
		std::shared_ptr<LightObject> m_CachedLight = nullptr;
	};
}