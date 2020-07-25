#include "Genesis/Input/InputManager.hpp"

namespace Genesis
{
	InputManager::InputManager(string config_folder_path)
	{
	}

	InputManager::~InputManager()
	{
	}

	bool InputManager::getButtonDown(string name)
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
	}


	bool InputManager::getButtonDown(fnv_hash32 hash)
	{
		if (has_value(this->button_states, hash))
		{
			return this->button_states[hash].current_state;
		}

		return false;
	}

	bool InputManager::getButtonPressed(fnv_hash32 hash)
	{
		if (has_value(this->button_states, hash))
		{
			return this->button_states[hash].current_state && !this->button_states[hash].previous_state;
		}

		return false;
	}

	void InputManager::updateButtonState(fnv_hash32 hash, bool state, Timestamp timestamp)
	{
		this->button_states[hash].timestamp = timestamp;
		if (state)
		{
			this->button_states[hash].current_state++;
		}
		else
		{
			this->button_states[hash].current_state--;
		}
	}
}