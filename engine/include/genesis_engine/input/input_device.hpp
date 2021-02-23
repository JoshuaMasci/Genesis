#pragma once

namespace genesis
{
	class InputDevice
	{
	protected:
		string name;
		InputManager* input_manager = nullptr;
	
	public:
		InputDevice(const string& device_name, InputManager* manager):name(device_name), input_manager(manager) {};

		const string& getName() { return this->name; };
	};
}