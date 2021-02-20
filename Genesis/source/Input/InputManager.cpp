#include "Genesis/Input/InputManager.hpp"

namespace Genesis
{
	InputManager::InputManager(string config_folder_path)
	{
	}

	InputManager::~InputManager()
	{
	}

	/*bool InputManager::getButtonDown(string name)
	{
		for (auto device : this->devices)
		{
			if (device->getButtonDown(name))
			{
				return true;
			}
		}

		return false;
	}

	bool InputManager::getButtonPressed(string name)
	{
		for (auto device : this->devices)
		{
			if (device->getButtonPressed(name))
			{
				return true;
			}
		}

		return false;
	}

	double InputManager::getAxis(string name)
	{
		AxisValue value;

		for (auto device : this->devices)
		{
			AxisValue temp = device->getAxis(name);
			if (temp.timestamp > value.timestamp)
			{
				value = temp;
			}
		}

		return value.value;
	}

	double InputManager::getButtonAxisCombo(string axis_name, string pos_button_name, string neg_button_name, bool clamp_value)
	{
		bool pos_button = this->getButtonDown(pos_button_name);
		bool neg_button = this->getButtonDown(neg_button_name);

		if (pos_button && neg_button)
		{
			return 0.0;
		}
		else if (pos_button && !neg_button)
		{
			return 1.0;
		}
		else if (!pos_button && neg_button)
		{
			return -1.0;
		}
		else
		{
			double value = this->getAxis(axis_name);

			if (clamp_value)
			{
				if (value > 1.0)
				{
					value = 1.0;
				}
				else if (value < -1.0)
				{
					value = -1.0;
				}
			}

			return value;
		}
	}

	void InputManager::update()
	{
		for (auto device : this->devices)
		{
			device->updateValues();
		}

		for (auto button : this->button_states)
		{
			button.second.previous_state = button.second.current_state;
		}
	}

	void InputManager::addInputDevice(InputDevice* device)
	{
		this->devices.emplace(device);
	}

	void InputManager::removeInputDevice(InputDevice* device)
	{
		this->devices.erase(device);
	}

	void InputManager::setMouseDevice(MouseDevice* device)
	{
		GENESIS_ENGINE_ASSERT(this->mouse_device == nullptr, "Mouse device already set");
		this->mouse_device = device;
		this->addInputDevice(this->mouse_device);
	}
	void InputManager::removeMouseDevice()
	{
		GENESIS_ENGINE_ASSERT(this->mouse_device != nullptr, "Mouse device null");
		this->removeInputDevice(this->mouse_device);
		this->mouse_device = nullptr;
	}

	void InputManager::setKeyboardDevice(KeyboardDevice* device)
	{
		GENESIS_ENGINE_ASSERT(this->keyboard_device == nullptr, "Keyboard device already set");
		this->keyboard_device = device;
		this->addInputDevice(this->keyboard_device);
	}

	void InputManager::removeKeyboardDevice()
	{
		GENESIS_ENGINE_ASSERT(this->keyboard_device != nullptr, "Keyboard device null");
		this->removeInputDevice(this->keyboard_device);
		this->keyboard_device = nullptr;
	}

	void InputManager::setMousePosition(vector2F position)
	{
		this->current_mouse_position = position;
	}

	vector2F InputManager::getMousePosition()
	{
		return this->current_mouse_position;
	}*/

	bool InputManager::getKeyboardDown(KeyboardButton button)
	{
		GENESIS_ENGINE_ASSERT(button < KeyboardButton::SIZE, "Keyboard button out of range");
		return this->keyboard_buttons[(size_t)button].current_state;
	}

	bool InputManager::getKeyboardPressed(KeyboardButton button)
	{
		GENESIS_ENGINE_ASSERT(button < KeyboardButton::SIZE, "Keyboard button out of range");
		return this->keyboard_buttons[(size_t)button].current_state && !this->keyboard_buttons[(size_t)button].previous_state;
	}

	void InputManager::updateKeyboardState(KeyboardButton button, bool state)
	{
		GENESIS_ENGINE_ASSERT(button < KeyboardButton::SIZE, "Keyboard button out of range");
		this->keyboard_buttons[(size_t)button].current_state = state;
	}

	string& InputManager::getInputText()
	{
		return this->keyboard_input_text;
	}

	void InputManager::updateInputText(const string& text)
	{
		this->keyboard_input_text = text;
	}

	bool InputManager::getMouseDown(MouseButton button)
	{
		GENESIS_ENGINE_ASSERT(button < MouseButton::SIZE, "Mouse button out of range");
		return this->mouse_buttons[(size_t)button].current_state;
	}

	bool InputManager::getMousePressed(MouseButton button)
	{
		GENESIS_ENGINE_ASSERT(button < MouseButton::SIZE, "Mouse button out of range");
		return this->mouse_buttons[(size_t)button].current_state && !this->mouse_buttons[(size_t)button].previous_state;
	}

	void InputManager::updateMouseState(MouseButton button, bool state)
	{
		GENESIS_ENGINE_ASSERT(button < MouseButton::SIZE, "Mouse button out of range");
		this->mouse_buttons[(size_t)button].current_state = state;
	}

	vector2F InputManager::getMousePosition()
	{
		return this->mouse_postion;
	}

	void InputManager::updateMousePosition(const vector2F& position)
	{
		this->mouse_postion = position;
	}

	bool InputManager::getButtonDown(fnv_hash32 string_hash)
	{
		auto it = this->button_values.find(string_hash);
		if (it != this->button_values.end())
		{
			return it->second.current_state;
		}

		return false;
	}

	bool InputManager::getButtonPressed(fnv_hash32 string_hash)
	{
		auto it = this->button_values.find(string_hash);
		if (it != this->button_values.end())
		{
			return it->second.current_state && !it->second.previous_state;
		}

		return false;
	}

	float InputManager::getAxis(fnv_hash32 string_hash)
	{
		auto it = this->axis_values.find(string_hash);
		if (it != this->axis_values.end())
		{
			return it->second.value;
		}

		return 0.0f;
	}

	float InputManager::getButtonAxis(fnv_hash32 axis, fnv_hash32 pos_button, fnv_hash32 neg_button, bool clamp_value)
	{
		bool pos_value = this->getButtonDown(pos_button);
		bool neg_value = this->getButtonDown(neg_button);

		if (pos_value && neg_value)
		{
			return 0.0;
		}
		else if (pos_value && !neg_value)
		{
			return 1.0;
		}
		else if (!pos_value && neg_value)
		{
			return -1.0;	
		}

		float value = this->getAxis(axis);

		if (clamp_value)
		{
			glm::clamp(value, -1.0f, 1.0f);
		}

		return value;
	}

	void InputManager::update()
	{
		for (size_t i = 0; i < (size_t)KeyboardButton::SIZE; i++)
		{
			this->keyboard_buttons[i].previous_state = this->keyboard_buttons[i].current_state;
		}

		for (size_t i = 0; i < (size_t)MouseButton::SIZE; i++)
		{
			this->mouse_buttons[i].previous_state = this->mouse_buttons[i].current_state;
		}

		for (auto button : this->button_values)
		{
			button.second.previous_state = button.second.current_state;
		}
	}

	void InputManager::updateButtonValue(fnv_hash32 string_hash, bool state)
	{
		this->button_values[string_hash].current_state = state;
	}

	void InputManager::updateAxisValue(fnv_hash32 string_hash, float value)
	{
		this->axis_values[string_hash].value = value;
	}
}