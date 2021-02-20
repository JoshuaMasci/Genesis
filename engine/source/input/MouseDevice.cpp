#include "genesis_engine/input/MouseDevice.hpp"

#include "genesis_engine/input/InputManager.hpp"

namespace genesis_engine
{	
	MouseDevice::MouseDevice(const string& device_name, InputManager* manager)
		:ArrayInputDevice(device_name, manager, (uint16_t)MouseButton::SIZE, 2)
	{
	}

	void MouseDevice::addMouseBinding(MouseButton button, fnv_hash32 string_hash)
	{
		this->addButtonBinding((size_t)button, string_hash);
	}

	void MouseDevice::removeMouseBinding(MouseButton button, fnv_hash32 string_hash)
	{
		this->removeButtonBinding((size_t)button, string_hash);
	}

	void MouseDevice::updateMouseState(MouseButton button, bool state)
	{
		this->input_manager->updateMouseState(button, state);
		this->updateButtonValue((size_t)button, state);
	}

	void MouseDevice::updateMousePosition(const vec2f& position)
	{
		this->input_manager->updateMousePosition(position);
	}
}