#include "genesis_engine/input/JoystickDevice.hpp"

namespace Genesis
{
	/*JoystickDevice::JoystickDevice(string name, uint16_t number_of_buttons, uint16_t number_of_hats, uint16_t number_of_axes)
		:ArrayInputDevice(name, number_of_buttons + (number_of_hats * 4), number_of_axes)
	{
		this->number_of_buttons = number_of_buttons;
	}

	JoystickDevice::~JoystickDevice()
	{
	}

	//A wrapper for addButton
	void JoystickDevice::addHatButton(string name, uint16_t index, HatDirection state)
	{
		uint16_t true_index = this->number_of_buttons + (index * 4) + (uint16_t)state;
		this->addButton(name, true_index);
	}

	void JoystickDevice::updateHat(uint16_t index, HatDirection state, Timestamp time)
	{
		uint16_t true_index = this->number_of_buttons + (index * 4);

		//Clear Values to start with
		this->updateButton(true_index + (uint16_t)HatDirection::Up, false, time); //Up
		this->updateButton(true_index + (uint16_t)HatDirection::Right, false, time); //Right
		this->updateButton(true_index + (uint16_t)HatDirection::Down, false, time); //Down
		this->updateButton(true_index + (uint16_t)HatDirection::Left, false, time); //Left

		switch (state)
		{
		case HatDirection::Centered:
			break;
		case HatDirection::Up:
			this->updateButton(true_index + (uint16_t)HatDirection::Up, true, time); //Up
			break;
		case HatDirection::Right:
			this->updateButton(true_index + (uint16_t)HatDirection::Right, true, time); //Right
			break;
		case HatDirection::Down:
			this->updateButton(true_index + (uint16_t)HatDirection::Down, true, time); //Down
			break;
		case HatDirection::Left:
			this->updateButton(true_index + (uint16_t)HatDirection::Left, true, time); //Left
			break;
		case HatDirection::RightUp:
			this->updateButton(true_index + (uint16_t)HatDirection::Up, true, time); //Up
			this->updateButton(true_index + (uint16_t)HatDirection::Right, true, time); //Right
			break;
		case HatDirection::RightDown:
			this->updateButton(true_index + (uint16_t)HatDirection::Right, true, time); //Right
			this->updateButton(true_index + (uint16_t)HatDirection::Down, true, time); //Down
			break;
		case HatDirection::LeftUp:
			this->updateButton(true_index + (uint16_t)HatDirection::Up, true, time); //Up
			this->updateButton(true_index + (uint16_t)HatDirection::Left, true, time); //Left
			break;
		case HatDirection::LeftDown:
			this->updateButton(true_index + (uint16_t)HatDirection::Down, true, time); //Down
			this->updateButton(true_index + (uint16_t)HatDirection::Left, true, time); //Left
			break;
		}
	}

	string JoystickDevice::getButtonName(uint16_t index)
	{
		if (index > this->number_of_buttons)
		{
			int temp = index - this->number_of_buttons;
			int hat_index = temp / 4;
			int hat_direction = temp % 4;
			return "Hat" + std::to_string(hat_index) + HatDirectionName[hat_direction];
		}

		return ArrayInputDevice::getButtonName(index);
	}*/


	JoystickDevice::JoystickDevice(const string& device_name, InputManager* manager, size_t number_of_buttons, size_t number_of_hats, size_t number_of_axes)
		:ArrayInputDevice(device_name, manager, number_of_buttons + (number_of_hats * 4), number_of_axes)
	{
		this->number_of_buttons = number_of_buttons;
	}

	void JoystickDevice::addHatBinding(size_t hat_index, HatDirection direction, fnv_hash32 binding)
	{
		size_t true_index = this->number_of_buttons + (hat_index * 4) + (uint16_t)direction;
		this->addButtonBinding(true_index, binding);
	}

	void JoystickDevice::removeHatBinding(size_t hat_index, HatDirection direction, fnv_hash32 binding)
	{
		size_t true_index = this->number_of_buttons + (hat_index * 4) + (uint16_t)direction;
		this->removeButtonBinding(true_index, binding);
	}

	void JoystickDevice::updateHatValue(size_t hat_index, HatDirection direction)
	{
		size_t true_index = this->number_of_buttons + (hat_index * 4);

		bool up_value = false;
		bool right_value = false;
		bool down_value = false;
		bool left_value = false;

		switch (direction)
		{
		case HatDirection::Up:
			up_value = true;
			break;
		case HatDirection::RightUp:
			right_value = true;
			up_value = true;
			break;
		case HatDirection::Right:
			right_value = true;
			break;
		case HatDirection::RightDown:
			right_value = true;
			down_value = true;
			break;
		case HatDirection::Down:
			down_value = true;
			break;
		case HatDirection::LeftDown:
			left_value = true;
			down_value = true;
			break;
		case HatDirection::Left:
			left_value = true;
			break;
		case HatDirection::LeftUp:
			left_value = true;
			up_value = true;
			break;
		}

		this->updateButtonValue(true_index + (size_t)HatButtons::Up, up_value);
		this->updateButtonValue(true_index + (size_t)HatButtons::Right, right_value);
		this->updateButtonValue(true_index + (size_t)HatButtons::Down, down_value);
		this->updateButtonValue(true_index + (size_t)HatButtons::Left, left_value);
	}
}