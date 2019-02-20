#pragma once

#include "Genesis/Core/Types.hpp"

#define INVALID_INDEX 255

namespace Genesis
{
	typedef uint32_t Timestamp;

	typedef uint16_t ButtonIndex;//May do settings later

	struct ButtonValue
	{
		bool current_value = 0;
		bool prev_value = 0;
		Timestamp timestamp = 0;
	};

	enum class AxisRange
	{
		FULL,
		FORWARD,
		BACKWARD
	};

	struct AxisValue
	{
		AxisValue(double value = 0.0, Timestamp time = 0) :value(value), timestamp(time) {};
		double value;
		Timestamp timestamp;
	};

	struct AxisSettings
	{
		AxisSettings(uint16_t axis_index, double deadzone = 0.0, bool inverted = false, AxisRange range = AxisRange::FULL, double sensitivity = 1.0, ButtonIndex pos = INVALID_INDEX, ButtonIndex neg = INVALID_INDEX)
		{
			this->axis_index = axis_index;
			this->deadzone = deadzone;
			this->inverted = inverted;
			this->range = range;
			this->sensitivity = sensitivity;
			this->positive_button = pos;
			this->negitive_button = neg;
		};

		uint16_t axis_index;
		double deadzone = 0.0;
		bool inverted = false;
		AxisRange range = AxisRange::FULL;
		double sensitivity = 1.0;
		ButtonIndex positive_button = INVALID_INDEX;
		ButtonIndex negitive_button = INVALID_INDEX;
	};
	
	class InputDevice
	{
	public:
		InputDevice(string name, uint16_t number_of_buttons, uint16_t number_of_axes);
		
		string getName() { return this->name; };

		void addButton(string name, ButtonIndex index);
		void addAxis(string name, AxisSettings settings);

		//Has a binding of that name
		bool hasButton(string name);
		//Returns if the button is currently being pressed
		bool getButtonDown(string name);
		//Returns true only once when the button is first pressed
		bool getButtonPressed(string name);

		bool hasAxis(string name);
		AxisValue getAxis(string name);

		void updateButton(uint16_t index, bool state, Timestamp time);
		void updateAxis(uint16_t index, double value, Timestamp time);

		virtual void updateValues();

		virtual string getButtonName(uint16_t index);
		virtual string getAxisName(uint16_t index);
	protected:
		string name;

		Array<ButtonValue> button_values;
		Array<AxisValue> axis_values;

		map<string, vector<ButtonIndex>> button_bindings;
		map<string, vector<AxisSettings>> axis_bindings;
	};
};