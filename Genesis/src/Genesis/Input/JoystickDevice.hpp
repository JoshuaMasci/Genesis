#pragma once

#include "Genesis/Input/InputDevice.hpp"

namespace Genesis
{
	enum class HatDirection
	{
		Up = 0,
		Right,
		Down,
		Left,
	};

	const string HatDirectionName[]
	{
		" Up",
		" Right",
		" Down",
		" Left"
	};

	enum class HatState
	{
		Centered,
		Up,
		Right,
		Down,
		Left,
		RightUp,
		RightDown,
		LeftUp,
		LeftDown,
	};

	class JoystickDevice: public InputDevice
	{
	public:
		JoystickDevice(string name, uint16_t number_of_buttons, uint16_t number_of_hats, uint16_t number_of_axes);
		~JoystickDevice();

		void addHatButton(string name, uint16_t index, HatDirection state);
		void updateHat(uint16_t index, HatState state, Timestamp time);

		virtual string getButtonName(uint16_t index) override;

	private:
		uint16_t number_of_buttons;
	};
};