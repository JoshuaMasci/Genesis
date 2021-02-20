#include "genesis_engine/input/ArrayInputDevice.hpp"

#include "genesis_engine/input/InputManager.hpp"

namespace genesis_engine
{
	float applyAxisSettings(float value, const AxisSettings& settings)
	{
		if (settings.inverted)
		{
			value *= -1.0f;
		}

		if (settings.range == AxisRange::FORWARD)
		{
			value = (value / 2.0f) + 0.5f;
		}
		else if (settings.range == AxisRange::BACKWARD)
		{
			value = (value / 2.0f) - 0.5f;
		}

		//apply deadzone
		if (fabs(value) > settings.deadzone)
		{
			float range = 1.0f - settings.deadzone;
			float sign = value / fabs(value);
			value = fabs(value) - settings.deadzone;
			value /= range;

			value *= sign;
			value *= settings.sensitivity;
		}
		else
		{
			value = 0.0f;
		}

		return value;
	}

	ArrayInputDevice::ArrayInputDevice(const string& device_name, InputManager* manager, size_t number_of_buttons, size_t number_of_axes)
		:InputDevice(device_name, manager)
	{
		this->button_settings.resize(number_of_buttons);
		this->axis_settings.resize(number_of_axes);
	}

	void ArrayInputDevice::addButtonBinding(size_t index, fnv_hash32 binding)
	{
		this->button_settings[index].push_back(binding);
	}

	void ArrayInputDevice::removeButtonBinding(size_t index, fnv_hash32 binding)
	{
		auto& settings = this->button_settings[index];
		for (size_t i = 0; i < settings.size(); i++)
		{
			if (settings[i] == binding)
			{
				settings.erase(settings.begin() + i);
			}
		}
	}

	void ArrayInputDevice::addAxisBinding(size_t index, fnv_hash32 binding, AxisSettings settings)
	{
		this->axis_settings[index].push_back( { binding, settings} );
	}

	void ArrayInputDevice::removeAxisBinding(size_t index, fnv_hash32 binding)
	{
		auto& settings = this->axis_settings[index];
		for (size_t i = 0; i < settings.size(); i++)
		{
			if (settings[i].binding == binding)
			{
				settings.erase(settings.begin() + i);
			}
		}
	}

	AxisSettings* genesis_engine::ArrayInputDevice::getAxisSettings(size_t index, fnv_hash32 binding)
	{
		auto& settings = this->axis_settings[index];
		for (size_t i = 0; i < settings.size(); i++)
		{
			if (settings[i].binding == binding)
			{
				return &settings[i].settings;
			}
		}

		return nullptr;
	}

	void ArrayInputDevice::updateButtonValue(size_t index, bool state)
	{
		auto& settings = this->button_settings[index];
		for (size_t i = 0; i < settings.size(); i++)
		{
			this->input_manager->updateButtonValue(settings[i], state);
		}
	}

	void ArrayInputDevice::updateAxisValue(size_t index, float value)
	{
		auto& settings = this->axis_settings[index];
		for (size_t i = 0; i < settings.size(); i++)
		{
			this->input_manager->updateAxisValue(settings[i].binding, applyAxisSettings(value, settings[i].settings));
		}
	}
}