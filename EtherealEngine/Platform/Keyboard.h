#pragma once
#include "Core/EtherealIncludes.h"
#include <array>
#include <optional>

namespace Ethereal
{
	class ETHEREAL_API Keyboard
	{
	public:
		static constexpr size_t KeyCount = 256;

		void OnKeyDown(unsigned char key);
		void OnKeyUp(unsigned char key);
		void OnChar(unsigned int character);

		bool GetKeyPressed(unsigned char key);
		std::optional<unsigned int> GetLastChar();
		bool IsKeyDown(unsigned char key) const;
		bool IsKeyUp(unsigned char key) const;

		void Reset();

	private:
		std::array<bool, KeyCount> s_KeyStates{};
		std::array<bool, KeyCount> s_KeyPressed{};
		std::optional<unsigned int> m_lastChar;
	};
}