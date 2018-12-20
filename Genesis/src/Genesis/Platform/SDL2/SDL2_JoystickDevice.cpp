#include "SDL2_JoystickDevice.hpp"

using namespace Genesis;

SDL2_JoystickDevice::SDL2_JoystickDevice(string joystick_name, SDL_Joystick* joystick)
	:InputDevice(joystick_name)
{
	this->joystick = joystick;
}

SDL2_JoystickDevice::~SDL2_JoystickDevice()
{
}

bool SDL2_JoystickDevice::hasAxis(string axis_name)
{
	return false;
}

double SDL2_JoystickDevice::getAxis(string axis_name)
{
	return 0.0;
}

bool SDL2_JoystickDevice::hasButton(string button_name)
{
	return false;
}

bool SDL2_JoystickDevice::getButton(string button_name)
{
	return false;
}
