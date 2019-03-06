#include "KeyboardDevice.hpp"

using namespace Genesis;

KeyboardDevice::KeyboardDevice(string name)
	:ArrayInputDevice(name, (uint16_t)KeyboardButton::SIZE, 0)
{
}

KeyboardDevice::~KeyboardDevice()
{
}

//A wrapper for updateButton
void Genesis::KeyboardDevice::updateKeyboardButton(KeyboardButton button, bool state, Timestamp time)
{
	if (button != KeyboardButton::SIZE)
	{
		this->updateButton((int32_t)button, state, time);
	}
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
