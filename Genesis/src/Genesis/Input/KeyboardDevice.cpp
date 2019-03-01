#include "KeyboardDevice.hpp"

using namespace Genesis;

KeyboardDevice::KeyboardDevice(string name)
	:InputDevice(name, 0, 0)
{
}

KeyboardDevice::~KeyboardDevice()
{
}

void KeyboardDevice::updateValues()
{
}

string KeyboardDevice::getAxisName(uint16_t index)
{
	return string();
}

string KeyboardDevice::getButtonName(uint16_t index)
{
	return string();
}
