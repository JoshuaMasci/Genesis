#include "InputDevice.hpp"

Genesis::InputDevice::InputDevice(string name, uint8_t number_of_button, uint8_t number_of_axis)
{
	this->name = name;
	this->button_values = Genesis::Array<ButtonValue>(number_of_button);
	this->axis_values = Genesis::Array<AxisValue>(number_of_axis);
}

void Genesis::InputDevice::addButton(string name, ButtonIndex index)
{
	this->button_bindings[name].push_back(index);
}

void Genesis::InputDevice::addAxis(string name, AxisSettings settings)
{
	this->axis_bindings[name].push_back(settings);
}

bool Genesis::InputDevice::hasButton(string name)
{
	return this->button_bindings.find(name) != this->button_bindings.end();
}

bool Genesis::InputDevice::getButtonDown(string name)
{
	if (this->hasButton(name))
	{
		for (ButtonIndex index : this->button_bindings[name])
		{
			if (index < this->button_values.size())
			{
				ButtonValue button = this->button_values[index];
				if (button.current_value == true)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Genesis::InputDevice::getButtonPressed(string name)
{
	if (this->hasButton(name))
	{
		for (ButtonIndex index : this->button_bindings[name])
		{
			if (index < this->button_values.size())
			{
				ButtonValue button = this->button_values[index];
				if (button.current_value == true && button.prev_value == false)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Genesis::InputDevice::hasAxis(string name)
{
	return this->axis_bindings.find(name) != this->axis_bindings.end();
}

double applyAxisSettings(double value, Genesis::AxisSettings settings)
{
	if (settings.inverted)
	{
		value *= -1;
	}

	if (settings.range == Genesis::AxisRange::FORWARD)
	{
		value = (value / 2.0) + 0.5;
	}
	else if (settings.range == Genesis::AxisRange::BACKWARD)
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

Genesis::AxisValue Genesis::InputDevice::getAxis(string name)
{
	if (this->hasAxis(name))
	{
		//Get most recent values
		AxisSettings axis = this->axis_bindings[name][0];
		Timestamp timestamp = this->axis_values[axis.axis_index].timestamp;

		for (size_t i = 0; i < this->axis_bindings[name].size(); i++)
		{
			AxisSettings temp = this->axis_bindings[name][i];
			if (this->axis_values[temp.axis_index].timestamp > timestamp)
			{
				axis = temp;
				timestamp = this->axis_values[temp.axis_index].timestamp;
			}
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

void Genesis::InputDevice::updateValues()
{
	for (int i = 0; i < this->button_values.size(); i++)
	{
		this->button_values[i].prev_value = this->button_values[i].current_value;
	}
}

void Genesis::InputDevice::updateButton(int32_t index, bool state, Timestamp time)
{
	this->button_values[index].current_value = state;
	this->button_values[index].timestamp = time;
}

void Genesis::InputDevice::updateAxis(int32_t index, double value, Timestamp time)
{
	this->axis_values[index].value = value;
	this->axis_values[index].timestamp = time;
}
