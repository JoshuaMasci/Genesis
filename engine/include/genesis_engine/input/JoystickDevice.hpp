#pragma once

#include "genesis_engine/input/ArrayInputDevice.hpp"

namespace genesis_engine
{
	enum class HatButtons
	{
		Up = 0,
		Right = 1,
		Down = 2,
		Left = 3,
	};

	const string HatDirectionName[]
	{
		" Up",
		" Right",
		" Down",
		" Left"
	};

	enum class HatDirection
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

	class JoystickDevice: public ArrayInputDevice
	{
	public:
		JoystickDevice(const string& device_name, InputManager* manager, size_t number_of_buttons, size_t number_of_hats, size_t number_of_axes);

		void addHatBinding(size_t hat_index, HatDirection direction, fnv_hash32 binding);
		void removeHatBinding(size_t hat_index, HatDirection direction, fnv_hash32 binding);

		void updateHatValue(size_t hat_index, HatDirection direction);

	protected:
		size_t number_of_buttons;
	};
};