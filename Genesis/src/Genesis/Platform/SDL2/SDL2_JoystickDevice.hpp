#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Input/InputDevice.hpp"

namespace Genesis
{
	class SDL2_JoystickDevice : public InputDevice
	{
	public:
		SDL2_JoystickDevice(string joystick_name, int32_t joystick_id);
		virtual ~SDL2_JoystickDevice();

	private:
		int32_t joystick_id;

		//JoystickAxisValue device_axis_values[32];
		//JoystickButtonValue device_button_values[32];
	};
};