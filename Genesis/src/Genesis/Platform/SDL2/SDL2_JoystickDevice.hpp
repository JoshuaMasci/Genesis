#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Input/InputDevice.hpp"
#include "Genesis/Platform/SDL2/SDL2_Include.hpp"

namespace Genesis
{
	class SDL2_JoystickDevice : public InputDevice
	{
	public:
		SDL2_JoystickDevice(string joystick_name, SDL_Joystick* joystick);
		virtual ~SDL2_JoystickDevice();

		// Inherited via InputDevice
		virtual bool hasAxis(string axis_name) override;
		virtual double getAxis(string axis_name) override;

		virtual bool hasButton(string button_name) override;
		virtual bool getButton(string button_name) override;

	private:
		struct JoystickAxisValue
		{
			Uint32 timestamp = 0;

			int16_t current_value = 0;
		};

		struct JoystickButtonValue
		{
			Uint32 timestamp = 0;

			Uint32 time_down = 0;
			uint8_t current_value = 0;
			uint8_t prev_value = 0;
		};

		SDL_Joystick* joystick = nullptr;

		JoystickAxisValue device_axis_values[32];
		JoystickButtonValue device_button_values[32];
	};
};