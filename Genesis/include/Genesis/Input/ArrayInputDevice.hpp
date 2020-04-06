#pragma once

#include "Genesis/Input/InputDevice.hpp"

#define INVALID_INDEX 65535

namespace Genesis
{
	typedef uint32_t Timestamp;

	typedef uint16_t ButtonIndex;//May do settings later

	enum class AxisRange
	{
		FULL,
		FORWARD,
		BACKWARD
	};

	struct AxisSettings
	{
		AxisSettings(uint16_t axis_index = INVALID_INDEX, double deadzone = 0.0, bool inverted = false, AxisRange range = AxisRange::FULL, double sensitivity = 1.0)
		{
			this->axis_index = axis_index;
			this->deadzone = deadzone;
			this->inverted = inverted;
			this->range = range;
			this->sensitivity = sensitivity;
		};

		uint16_t axis_index;
		double deadzone = 0.0;
		bool inverted = false;
		AxisRange range = AxisRange::FULL;
		double sensitivity = 1.0;
	};
	
	class ArrayInputDevice : public InputDevice
	{
	public:
		ArrayInputDevice(string device_name, uint16_t number_of_buttons, uint16_t number_of_axes);

		//Has a binding of that name
		virtual bool hasButton(string button_name);
		virtual ButtonValue getButton(string button_name);
		//Returns if the button is currently being pressed
		virtual bool getButtonDown(string button_name);
		//Returns true only once when the button is first pressed
		virtual bool getButtonPressed(string button_name);

		virtual bool hasAxis(string axis_name);
		virtual AxisValue getAxis(string axis_name);

		void addButton(string button_name, ButtonIndex index);
		void addAxis(string axis_name, AxisSettings settings);

		void updateButton(uint16_t index, bool state, Timestamp time);
		void updateAxis(uint16_t index, double value, Timestamp time);

		virtual void updateValues();

		virtual string getButtonName(uint16_t index);
		virtual string getAxisName(uint16_t index);
	protected:

		vector<ButtonValue> button_values;
		vector<AxisValue> axis_values;

		map<string, ButtonIndex> button_bindings;
		map<string, AxisSettings> axis_bindings;
	};
};