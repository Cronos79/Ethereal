#pragma once
#include "Core/EtherealIncludes.h"
#include <array>
#include <queue>
#include <optional>

namespace Ethereal
{
	class ETHEREAL_API Keyboard
	{
		friend class EEWindows;
	public:
		static constexpr size_t KeyCount = 256;

		class Event
		{
		public:
			enum class Type
			{
				Press,
				Release,
				Char
			};

			Event(Type type, unsigned char keycode)
				: m_Type(type), m_KeyCode(keycode), m_Char(0)
			{
			}

			Event(Type type, unsigned int character)
				: m_Type(type), m_KeyCode(0), m_Char(character)
			{
			}

			Type GetType() const
			{
				return m_Type;
			}
			unsigned char GetKeyCode() const
			{
				return m_KeyCode;
			}
			unsigned int GetChar() const
			{
				return m_Char;
			}

		private:
			Type m_Type;
			unsigned char m_KeyCode;
			unsigned int m_Char;
		};

	public:
		void OnKeyDown(unsigned char key);
		void OnKeyUp(unsigned char key);
		void OnChar(unsigned int character);

		bool GetKeyPressed(unsigned char key);
		std::optional<unsigned int> GetLastChar();
		bool IsKeyDown(unsigned char key) const;
		bool IsKeyUp(unsigned char key) const;

		std::optional<Event> ReadEvent();
		bool EventBufferEmpty() const;
		void FlushEvents();

		void Reset();

	private:
		void TrimEventBuffer();

	private:
		std::array<bool, KeyCount> s_KeyStates{};
		std::array<bool, KeyCount> s_KeyPressed{};
		std::optional<unsigned int> m_lastChar;

		static constexpr size_t EventBufferSize = 16;
		std::queue<Event> m_EventBuffer;
	};
}