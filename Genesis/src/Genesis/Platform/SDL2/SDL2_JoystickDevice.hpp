#pragma once

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
		SDL_Joystick* joystick = nullptr;
	};
};