#include "EETimer.h"

namespace Ethereal
{
	EETimer::EETimer()
	{
		m_lastTime = clock::now();
	}

	void EETimer::Tick()
	{
		auto currentTime = clock::now();
		std::chrono::duration<float> elapsed = currentTime - m_lastTime;
		m_deltaTime = elapsed.count();
		m_lastTime = currentTime;

		// FPS calculation
		m_timeAccumulator += m_deltaTime;
		++m_frameCount;

		if (m_timeAccumulator >= 1.0f)
		{
			m_fps = static_cast<float>(m_frameCount) / m_timeAccumulator;
			m_frameCount = 0;
			m_timeAccumulator = 0.0f;
		}
	}

	float EETimer::GetDeltaTime() const
	{
		return m_deltaTime;
	}

	float EETimer::GetFPS() const
	{
		return m_fps;
	}
}