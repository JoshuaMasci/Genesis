#include "genesis_engine/input/KeyboardDevice.hpp"

#include "genesis_engine/input/InputManager.hpp"

namespace Genesis
{
	/*KeyboardDevice::KeyboardDevice(string name)
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
	}*/

	KeyboardDevice::KeyboardDevice(const string& device_name, InputManager* manager)
		:ArrayInputDevice(device_name, manager, (uint16_t)KeyboardButton::SIZE, 0)
	{
	}

	void KeyboardDevice::addKeyboardBinding(KeyboardButton button, fnv_hash32 string_hash)
	{
		this->addButtonBinding((size_t)button, string_hash);
	}

	void KeyboardDevice::removeKeyboardBinding(KeyboardButton button, fnv_hash32 string_hash)
	{
		this->removeButtonBinding((size_t)button, string_hash);
	}

	void KeyboardDevice::updateKeyboardState(KeyboardButton button, bool state)
	{
		this->input_manager->updateKeyboardState(button, state);
		this->updateButtonValue((size_t)button, state);
	}

	void KeyboardDevice::updateInputText(string input_text)
	{
		this->input_manager->updateInputText(input_text);
	}
}