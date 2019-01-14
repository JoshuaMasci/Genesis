#include "SDL2_JoystickDevice.hpp"

#include "Genesis/Platform/SDL2/SDL2_Include.hpp"

using namespace Genesis;

SDL2_JoystickDevice::SDL2_JoystickDevice(string joystick_name, int32_t joystick_id)
	:InputDevice(joystick_name)
{
	this->joystick_id = joystick_id;
}

SDL2_JoystickDevice::~SDL2_JoystickDevice()
{
}
