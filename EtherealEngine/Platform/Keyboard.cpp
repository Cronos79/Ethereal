#include "Platform/Keyboard.h"

namespace Ethereal
{
	void Keyboard::OnKeyDown(unsigned char key)
	{
		if (key < KeyCount)
		{
			if (!s_KeyStates[key])
				s_KeyPressed[key] = true;

			s_KeyStates[key] = true;
			m_EventBuffer.push(Event(Event::Type::Press, key));
			TrimEventBuffer();
		}
	}

	void Keyboard::OnKeyUp(unsigned char key)
	{
		if (key < KeyCount)
		{
			s_KeyStates[key] = false;
			m_EventBuffer.push(Event(Event::Type::Release, key));
			TrimEventBuffer();
		}
	}

	void Keyboard::OnChar(unsigned int character)
	{
		m_lastChar = character;
		m_EventBuffer.push(Event(Event::Type::Char, character));
		TrimEventBuffer();
	}

	bool Keyboard::GetKeyPressed(unsigned char key)
	{
		if (key < KeyCount && s_KeyPressed[key])
		{
			s_KeyPressed[key] = false;
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

	std::optional<Keyboard::Event> Keyboard::ReadEvent()
	{
		if (!m_EventBuffer.empty())
		{
			Event e = m_EventBuffer.front();
			m_EventBuffer.pop();
			return e;
		}
		return {};
	}

	bool Keyboard::EventBufferEmpty() const
	{
		return m_EventBuffer.empty();
	}

	void Keyboard::FlushEvents()
	{
		while (!m_EventBuffer.empty())
			m_EventBuffer.pop();
	}

	void Keyboard::TrimEventBuffer()
	{
		while (m_EventBuffer.size() > EventBufferSize)
		{
			m_EventBuffer.pop();
		}
	}

	void Keyboard::Reset()
	{
		s_KeyStates.fill(false);
		s_KeyPressed.fill(false);
		FlushEvents();
	}
}