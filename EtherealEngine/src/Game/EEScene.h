#pragma once
#include <vector>
#include <memory>
#include <string>
#include "EEGameObject.h"

namespace EtherealEngine
{
	class EEScene
	{
	public:
		EEScene();
		~EEScene();

		void InitScene();

		// Add a game object to the scene
		void AddGameObject(std::shared_ptr<EEGameObject> object);

		// Remove a game object by pointer (optional: by name or ID)
		void RemoveGameObject(std::shared_ptr<EEGameObject> object);

		// Update all game objects
		void Update(float deltaTime);

		// Draw all game objects
		void Draw();

		// Draw all game object UIs
		void DrawUI();

		// Accessors
		const std::vector<std::shared_ptr<EEGameObject>>& GetGameObjects() const
		{
			return m_GameObjects;
		}

		// Optional: find by name
		std::shared_ptr<EEGameObject> FindByName(const std::string& name) const;

	private:
		std::vector<std::shared_ptr<EEGameObject>> m_GameObjects;
	};
}