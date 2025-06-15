#include "Platform/Keyboard.h"

namespace Ethereal
{
	void Keyboard::OnKeyDown(unsigned char key)
	{
		if (key < KeyCount)
		{
			if (!s_KeyStates[key]) // Only set pressed if it wasn't already down
				s_KeyPressed[key] = true;
			s_KeyStates[key] = true;
		}
	}

	void Keyboard::OnKeyUp(unsigned char key)
	{
		if (key < KeyCount)
			s_KeyStates[key] = false;
	}

	void Keyboard::OnChar(unsigned int character)
	{
		m_lastChar = character;
	}

	bool Keyboard::GetKeyPressed(unsigned char key)
	{
		if (key < KeyCount && s_KeyPressed[key])
		{
			s_KeyPressed[key] = false; // Reset so it only fires once
			return true;
		}
		return false;
	}

	std::optional<unsigned int> Keyboard::GetLastChar()
	{
		auto result = m_lastChar;
		m_lastChar.reset(); 
		return result;
	}

	bool Keyboard::IsKeyDown(unsigned char key) const
	{
		if (key < KeyCount)
			return s_KeyStates[key];
		return false;
	}

	bool Keyboard::IsKeyUp(unsigned char key) const
	{
		if (key < KeyCount)
			return !s_KeyStates[key];
		return true;
	}

	void Keyboard::Reset()
	{
		s_KeyStates.fill(false);
		s_KeyPressed.fill(false);
	}
}