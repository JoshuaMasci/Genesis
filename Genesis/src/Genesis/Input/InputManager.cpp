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
