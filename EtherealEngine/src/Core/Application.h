#pragma once
#include <memory>
#include "Game/EEScene.h"

namespace EtherealEngine
{

	class Application
	{
	public:
		virtual ~Application() = default;

		int Run(); // Entry point

	protected:
		virtual bool Initialize();           // Called once at start
		virtual void Update(float deltaTime); // Called every frame
		virtual void Shutdown();            // Called once at end

	protected:
		std::unique_ptr<EEScene> m_CurrentScene;
	};

} // namespace EtherealEngine