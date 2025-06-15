#include "Platform/Mouse.h"

namespace Ethereal
{
	Mouse::Mouse() = default;

	void Mouse::OnMove(int x, int y)
	{
		m_x = x;
		m_y = y;
	}

	void Mouse::OnButtonDown(Button button)
	{
		if (button < ButtonCount)
		{
			if (!m_buttonStates[button])
				m_buttonPressed[button] = true;
			m_buttonStates[button] = true;
		}
	}

	void Mouse::OnButtonUp(Button button)
	{
		if (button < ButtonCount)
			m_buttonStates[button] = false;
	}

	void Mouse::OnWheel(int delta)
	{
		m_wheelDelta += delta;
	}

	int Mouse::GetX() const
	{
		return m_x;
	}
	int Mouse::GetY() const
	{
		return m_y;
	}
	std::pair<int, int> Mouse::GetPosition() const
	{
		return { m_x, m_y };
	}

	bool Mouse::IsButtonDown(Button button) const
	{
		return button < ButtonCount ? m_buttonStates[button] : false;
	}

	bool Mouse::IsButtonUp(Button button) const
	{
		return button < ButtonCount ? !m_buttonStates[button] : true;
	}

	bool Mouse::GetButtonPressed(Button button)
	{
		if (button < ButtonCount && m_buttonPressed[button])
		{
			m_buttonPressed[button] = false;
			return true;
		}
		return false;
	}

	int Mouse::GetWheelDelta()
	{
		int delta = m_wheelDelta;
		m_wheelDelta = 0;
		return delta;
	}

	void Mouse::Reset()
	{
		m_buttonStates.fill(false);
		m_buttonPressed.fill(false);
		m_wheelDelta = 0;
	}
}