#pragma once
#include <array>
#include "Platform/WinInclude.h"

namespace EtherealEngine
{
	class Mouse
	{
	public:
		enum Button
		{
			Left = 0,
			Right,
			Middle,
			ButtonCount
		};

		void OnButtonDown(Button button);
		void OnButtonUp(Button button);
		void OnMove(int x, int y);
		void Update();

		bool IsButtonDown(Button button) const;
		bool IsButtonPressed(Button button) const;
		bool IsButtonReleased(Button button) const;
		int GetX() const
		{
			return m_X;
		}
		int GetY() const
		{
			return m_Y;
		}

	private:
		std::array<bool, ButtonCount> m_Current{};
		std::array<bool, ButtonCount> m_Previous{};
		int m_X = 0;
		int m_Y = 0;
	};
}