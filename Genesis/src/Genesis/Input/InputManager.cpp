#include "InputManager.hpp"

Genesis::InputManager::InputManager(string config_folder_path)
{
}

Genesis::InputManager::~InputManager()
{
}

bool Genesis::InputManager::getButtonDown(string name)
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

bool Genesis::InputManager::getButtonPressed(string name)
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

double Genesis::InputManager::getAxis(string name)
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

void Genesis::InputManager::update()
{
	for (auto device : this->devices)
	{
		device->updateValues();
	}
}

void Genesis::InputManager::addInputDevice(InputDevice* device)
{
	this->devices.emplace(device);
}

void Genesis::InputManager::removeInputDevice(InputDevice* device)
{
	this->devices.erase(device);
}
