#pragma once
#include "Core/EtherealIncludes.h"
#include <vector>
#include "Assets/GameObject.h"
#include <stdint.h>
#include <memory>
#include "UI/EditorGui.h"

namespace Ethereal
{
	class ETHEREAL_API Scene
	{
	public:
		Scene();
		Scene(const std::string& name);
		virtual ~Scene();
		void AddGameObject(GameObject obj);
		void RemoveGameObject(GameObject obj);
		void RemoveGameObjectByName(const std::string& name);
		void RemoveGameObjectByID(int32_t id);
		virtual void HandleInput(float deltaTime);
		virtual void Update(float deltaTime);
		void DrawUI(float deltaTime);
		void Render();

		const std::vector<GameObject>& GetGameObjects() const;
		void SetName(const std::string& name);
		const std::string& GetName() const;

		void OnActivate();
		void OnDeactivate();	
	private:

	private:
		std::vector<GameObject> m_GameObjects;
		std::string m_Name;
		std::unique_ptr<EditorGui> m_Editor;
	};
}