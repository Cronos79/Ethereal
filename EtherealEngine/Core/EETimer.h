#pragma once
#include <chrono>
#include "EtherealIncludes.h"


namespace Ethereal
{
	class ETHEREAL_API EETimer
	{
	public:
		using clock = std::chrono::high_resolution_clock;

		EETimer();

		// Call this once per frame
		void Tick();

		// Time between last two ticks (in seconds)
		float GetDeltaTime() const;

		// Frames per second, updated every second
		float GetFPS() const;

	private:
		clock::time_point m_lastTime;
		float m_deltaTime{ 0.0f };

		// FPS calculation
		int m_frameCount{ 0 };
		float m_fps{ 0.0f };
		float m_timeAccumulator{ 0.0f };
	};
}