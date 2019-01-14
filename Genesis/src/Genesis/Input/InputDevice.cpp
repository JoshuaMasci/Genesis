#include "InputDevice.hpp"

Genesis::InputDevice::InputDevice(string name, uint8_t number_of_button, uint8_t number_of_axis)
{
	this->button_values = Genesis::Array<ButtonValue>(number_of_button);
	this->axis_values = Genesis::Array<AxisValue>(number_of_axis);
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

		double axis_value = this->axis_values[axis.axis_index].value;

		if (axis.inverted)
		{
			axis_value *= -1;
		}

		if (axis.range == AxisRange::FORWARD)
		{
			axis_value = (axis_value / 2.0) + 0.5;
		}
		else if (axis.range == AxisRange::BACKWARD)
		{
			axis_value = (axis_value / 2.0) - 0.5;
		}

		//apply deadzone
		if (fabs(axis_value) > axis.deadzone)
		{
			double range = 1.0 - axis.deadzone;
			double sign = axis_value / fabs(axis_value);
			axis_value = fabs(axis_value) - axis.deadzone;
			axis_value /= range;

			axis_value *= sign;

			return AxisValue(timestamp, axis_value * axis.sensitivity);
		}

	}

	return AxisValue();
}
