#include "InputManager.hpp"

using namespace Genesis;

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
			else if(value < -1.0)
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
