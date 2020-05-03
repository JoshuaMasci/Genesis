#include "Genesis/Input/KeyboardDevice.hpp"

using namespace Genesis;

KeyboardDevice::KeyboardDevice(string name)
	:ArrayInputDevice(name, (uint16_t)KeyboardButton::SIZE, 0)
{
}

KeyboardDevice::~KeyboardDevice()
{
}

void KeyboardDevice::updateValues()
{
	ArrayInputDevice::updateValues();
	this->input_text.clear();
}

//A wrapper for updateButton
void KeyboardDevice::updateKeyboardButton(KeyboardButton button, bool state, Timestamp time)
{
	if (button != KeyboardButton::SIZE)
	{
		this->updateButton((uint16_t)button, state, time);
	}
}

void KeyboardDevice::updateInputText(string input_text)
{
	this->input_text = input_text;
}

string KeyboardDevice::getInputText()
{
	return this->input_text;
}

bool KeyboardDevice::getButtonState(KeyboardButton button)
{
	return this->button_values[(size_t)button].current_value;
}

string KeyboardDevice::getAxisName(uint16_t index)
{
	return string();
}

string KeyboardDevice::getButtonName(uint16_t index)
{
	return string();
}
