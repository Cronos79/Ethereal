#pragma once
#include <array>
#include <Windows.h>

namespace EtherealEngine
{
	class Keyboard
	{
	public:
		static constexpr int KeyCount = 256;

		void OnKeyDown(uint8_t key);
		void OnKeyUp(uint8_t key);
		void Update();

		bool IsKeyDown(uint8_t key) const;
		bool IsKeyPressed(uint8_t key) const;
		bool IsKeyReleased(uint8_t key) const;

	private:
		std::array<bool, KeyCount> m_Current{};
		std::array<bool, KeyCount> m_Previous{};
	};
}