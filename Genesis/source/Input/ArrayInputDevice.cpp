#include "Genesis/Input/ArrayInputDevice.hpp"

using namespace Genesis;

ArrayInputDevice::ArrayInputDevice(string device_name, uint16_t number_of_buttons, uint16_t number_of_axes)
	:InputDevice(device_name)
{
	this->button_values = List<ButtonValue>(number_of_buttons);
	this->axis_values = List<AxisValue>(number_of_axes);
}

bool ArrayInputDevice::hasButton(string button_name)
{
	return this->button_bindings.find(button_name) != this->button_bindings.end();
}

ButtonValue ArrayInputDevice::getButton(string button_name)
{
	if (this->hasButton(button_name))
	{
		ButtonIndex index = this->button_bindings[button_name];
		return this->button_values[index];
	}

	return ButtonValue();
}

bool ArrayInputDevice::getButtonDown(string button_name)
{
	if (this->hasButton(button_name))
	{
		ButtonIndex index = this->button_bindings[button_name];
		ButtonValue button = this->button_values[index];
		if (button.current_value == true)
		{
			return true;
		}
	}

	return false;
}

bool ArrayInputDevice::getButtonPressed(string button_name)
{
	if (this->hasButton(button_name))
	{
		ButtonIndex index = this->button_bindings[button_name];
		ButtonValue button = this->button_values[index];
		if (button.current_value == true && button.prev_value == false)
		{
			return true;
		}
	}

	return false;
}

bool ArrayInputDevice::hasAxis(string axis_name)
{
	return this->axis_bindings.find(axis_name) != this->axis_bindings.end();
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

AxisValue ArrayInputDevice::getAxis(string axis_name)
{
	if (this->hasAxis(axis_name))
	{
		//Get most recent values
		AxisSettings axis = this->axis_bindings[axis_name];

		if (axis.axis_index == INVALID_INDEX)
		{
			return AxisValue();
		}

		AxisValue axis_value = this->axis_values[axis.axis_index];
		axis_value.value = applyAxisSettings(axis_value.value, axis);

		return axis_value;
	}

	return AxisValue();
}

void ArrayInputDevice::addButton(string button_name, ButtonIndex index)
{
	this->button_bindings[button_name] = index;
}

void ArrayInputDevice::addAxis(string axis_name, AxisSettings settings)
{
	this->axis_bindings[axis_name] = settings;
}

void ArrayInputDevice::updateButton(uint16_t index, bool state, Timestamp time)
{
	this->button_values[index].current_value = state;
	this->button_values[index].timestamp = time;
}

void ArrayInputDevice::updateAxis(uint16_t index, double value, Timestamp time)
{
	this->axis_values[index].value = value;
	this->axis_values[index].timestamp = time;
}

void ArrayInputDevice::updateValues()
{
	for (int i = 0; i < this->button_values.size(); i++)
	{
		this->button_values[i].prev_value = this->button_values[i].current_value;
	}
}

string ArrayInputDevice::getButtonName(uint16_t index)
{
	return "Button " + std::to_string(index);
}

string ArrayInputDevice::getAxisName(uint16_t index)
{
	return "Axis " + std::to_string(index);
}
