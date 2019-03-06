#include "JoystickDevice.hpp"

using namespace Genesis;

JoystickDevice::JoystickDevice(string name, uint16_t number_of_buttons, uint16_t number_of_hats, uint16_t number_of_axes)
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
	uint16_t true_index = this->number_of_buttons + (index * 4) + (uint16_t) state;
	this->addButton(name, true_index);
}

void JoystickDevice::updateHat(uint16_t index, HatState state, Timestamp time)
{
	uint16_t true_index = this->number_of_buttons + (index * 4);
	
	//Clear Values to start with
	this->updateButton(true_index + (uint16_t)HatDirection::Up, false, time); //Up
	this->updateButton(true_index + (uint16_t)HatDirection::Right, false, time); //Right
	this->updateButton(true_index + (uint16_t)HatDirection::Down, false, time); //Down
	this->updateButton(true_index + (uint16_t)HatDirection::Left, false, time); //Left

	switch(state)
	{
	case HatState::Centered:
		break;
	case HatState::Up:
		this->updateButton(true_index + (uint16_t)HatDirection::Up, true, time); //Up
		break;
	case HatState::Right:
		this->updateButton(true_index + (uint16_t)HatDirection::Right, true, time); //Right
		break;
	case HatState::Down:
		this->updateButton(true_index + (uint16_t)HatDirection::Down, true, time); //Down
		break;
	case HatState::Left:
		this->updateButton(true_index + (uint16_t)HatDirection::Left, true, time); //Left
		break;
	case HatState::RightUp:
		this->updateButton(true_index + (uint16_t)HatDirection::Up, true, time); //Up
		this->updateButton(true_index + (uint16_t)HatDirection::Right, true, time); //Right
		break;
	case HatState::RightDown:
		this->updateButton(true_index + (uint16_t)HatDirection::Right, true, time); //Right
		this->updateButton(true_index + (uint16_t)HatDirection::Down, true, time); //Down
		break;
	case HatState::LeftUp:
		this->updateButton(true_index + (uint16_t)HatDirection::Up, true, time); //Up
		this->updateButton(true_index + (uint16_t)HatDirection::Left, true, time); //Left
		break;
	case HatState::LeftDown:
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
}
