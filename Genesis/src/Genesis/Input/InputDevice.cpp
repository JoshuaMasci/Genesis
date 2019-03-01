#include "InputDevice.hpp"

using namespace Genesis;

InputDevice::InputDevice(string name, uint16_t number_of_buttons, uint16_t number_of_axes)
{
	this->name = name;
	this->button_values = Array<ButtonValue>(number_of_buttons);
	this->axis_values = Array<AxisValue>(number_of_axes);
}

void InputDevice::addButton(string name, ButtonIndex index)
{
	this->button_bindings[name] = index;
}

void InputDevice::addAxis(string name, AxisSettings settings)
{
	this->axis_bindings[name] = settings;
}

bool InputDevice::hasButton(string name)
{
	return this->button_bindings.find(name) != this->button_bindings.end();
}

bool InputDevice::getButtonDown(string name)
{
	if (this->hasButton(name))
	{
		ButtonIndex index = this->button_bindings[name];
		ButtonValue button = this->button_values[index];
		if (button.current_value == true)
		{
			return true;
		}
	}

	return false;
}

bool InputDevice::getButtonPressed(string name)
{
	if (this->hasButton(name))
	{
		ButtonIndex index = this->button_bindings[name];
		ButtonValue button = this->button_values[index];
		if (button.current_value == true && button.prev_value == false)
		{
			return true;
		}
	}

	return false;
}

bool InputDevice::hasAxis(string name)
{
	return this->axis_bindings.find(name) != this->axis_bindings.end();
}

double applyAxisSettings(double value, AxisSettings settings)
{
	if (settings.inverted)
	{
		value *= -1;
	}

	if (settings.range == AxisRange::FORWARD)
	{
		value = (value / 2.0) + 0.5;
	}
	else if (settings.range == AxisRange::BACKWARD)
	{
		value = (value / 2.0) - 0.5;
	}

	//apply deadzone
	if (fabs(value) > settings.deadzone)
	{
		double range = 1.0 - settings.deadzone;
		double sign = value / fabs(value);
		value = fabs(value) - settings.deadzone;
		value /= range;

		value *= sign;
		value *= settings.sensitivity;
	}
	else
	{
		value = 0.0;
	}

	return value;
}

AxisValue InputDevice::getAxis(string name)
{
	if (this->hasAxis(name))
	{
		//Get most recent values
		AxisSettings axis = this->axis_bindings[name];

		if (axis.axis_index == INVALID_INDEX)
		{
			return AxisValue();
		}

		AxisValue axis_value = this->axis_values[axis.axis_index];
		axis_value.value = applyAxisSettings(axis_value.value, axis);

		if (axis.positive_button != INVALID_INDEX && this->button_values[axis.positive_button].current_value && this->button_values[axis.positive_button].timestamp > axis_value.timestamp)
		{
			axis_value.value = 1.0;
			axis_value.timestamp = this->button_values[axis.positive_button].timestamp;
		}

		if (axis.negitive_button != INVALID_INDEX && this->button_values[axis.negitive_button].current_value && this->button_values[axis.negitive_button].timestamp > axis_value.timestamp)
		{
			axis_value.value = -1.0;
			axis_value.timestamp = this->button_values[axis.negitive_button].timestamp;
		}

		return axis_value;
	}

	return AxisValue();
}

void InputDevice::updateButton(uint16_t index, bool state, Timestamp time)
{
	this->button_values[index].current_value = state;
	this->button_values[index].timestamp = time;
}

void InputDevice::updateAxis(uint16_t index, double value, Timestamp time)
{
	this->axis_values[index].value = value;
	this->axis_values[index].timestamp = time;
}

void InputDevice::updateValues()
{
	for (int i = 0; i < this->button_values.size(); i++)
	{
		this->button_values[i].prev_value = this->button_values[i].current_value;
	}
}

string Genesis::InputDevice::getButtonName(uint16_t index)
{
	return "Button " + std::to_string(index);
}

string Genesis::InputDevice::getAxisName(uint16_t index)
{
	return "Axis " + std::to_string(index);
}
