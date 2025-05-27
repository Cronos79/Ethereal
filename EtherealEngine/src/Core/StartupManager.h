#pragma once

namespace EtherealEngine
{
	struct WindowSettings;

	class StartupManager
	{
	public:
		StartupManager() = default;
		bool Initialize();     // Full engine startup sequence
		void Shutdown();       // Clean up systems

	private:
		WindowSettings LoadWindowSettings();
	};
}