#include "pch.h"
#include "Mouse.h"

namespace EtherealEngine
{
	void Mouse::OnButtonDown(Button button)
	{
		if (button < ButtonCount)
			m_Current[button] = true;
	}

	void Mouse::OnButtonUp(Button button)
	{
		if (button < ButtonCount)
			m_Current[button] = false;
	}

	void Mouse::OnMove(int x, int y)
	{
		m_X = x;
		m_Y = y;
	}

	void Mouse::Update()
	{
		m_Previous = m_Current;
	}

	bool Mouse::IsButtonDown(Button button) const
	{
		return button < ButtonCount && m_Current[button];
	}

	bool Mouse::IsButtonPressed(Button button) const
	{
		return button < ButtonCount && m_Current[button] && !m_Previous[button];
	}

	bool Mouse::IsButtonReleased(Button button) const
	{
		return button < ButtonCount && !m_Current[button] && m_Previous[button];
	}
}