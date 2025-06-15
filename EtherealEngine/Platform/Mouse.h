#pragma once
#include "Core/EtherealIncludes.h"
#include <array>
#include <optional>

namespace Ethereal
{
	class ETHEREAL_API Mouse
	{
	public:
		enum Button
		{
			Left = 0,
			Right = 1,
			Middle = 2,
			ButtonCount = 3
		};

		Mouse();

		// Call from WindowProc on mouse events
		void OnMove(int x, int y);
		void OnButtonDown(Button button);
		void OnButtonUp(Button button);
		void OnWheel(int delta);

		// Query methods
		int GetX() const;
		int GetY() const;
		std::pair<int, int> GetPosition() const;

		bool IsButtonDown(Button button) const;
		bool IsButtonUp(Button button) const;
		bool GetButtonPressed(Button button); // Edge-triggered, like Keyboard::GetKeyPressed

		int GetWheelDelta(); // Returns and resets the wheel delta since last call

		void Reset();

	private:
		int m_x = 0;
		int m_y = 0;
		std::array<bool, ButtonCount> m_buttonStates{};
		std::array<bool, ButtonCount> m_buttonPressed{};
		int m_wheelDelta = 0;
	};
}
