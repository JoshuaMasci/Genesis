#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	typedef uint32_t Timestamp;

	typedef uint8_t ButtonIndex;//May do settings later

	struct ButtonValue
	{
		bool current_value = 0;
		bool prev_value = 0;
		Timestamp timestamp = 0;
	};

	enum AxisRange
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
		uint8_t axis_index;
		double sensitivity = 1.0;
		double deadzone = 0.0;
		bool inverted = false;
		AxisRange range = AxisRange::FULL;
	};
	
	class InputDevice
	{
	public:
		InputDevice(string name, uint8_t number_of_button, uint8_t number_of_axis);
		
		string getName() { return this->name; };

		//Has a binding of that name
		bool hasButton(string name);
		//Returns if the button is currently being pressed
		bool getButtonDown(string name);
		//Returns true only once when the button is first pressed
		bool getButtonPressed(string name);

		bool hasAxis(string name);
		AxisValue getAxis(string name);

	private:
		string name;

		Array<ButtonValue> button_values;
		Array<AxisValue> axis_values;

		map<string, vector<ButtonIndex>> button_bindings;
		map<string, vector<AxisSettings>> axis_bindings;
	};
};