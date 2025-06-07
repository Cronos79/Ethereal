#include "pch.h"
#include "Keyboard.h"

namespace EtherealEngine
{
	void Keyboard::OnKeyDown(uint8_t key)
	{
		if (key < KeyCount)
			m_Current[key] = true;
	}

	void Keyboard::OnKeyUp(uint8_t key)
	{
		if (key < KeyCount)
			m_Current[key] = false;
	}

	void Keyboard::Update()
	{
		m_Previous = m_Current;
	}

	bool Keyboard::IsKeyDown(uint8_t key) const
	{
		return key < KeyCount && m_Current[key];
	}

	bool Keyboard::IsKeyPressed(uint8_t key) const
	{
		return key < KeyCount && m_Current[key] && !m_Previous[key];
	}

	bool Keyboard::IsKeyReleased(uint8_t key) const
	{
		return key < KeyCount && !m_Current[key] && m_Previous[key];
	}
}