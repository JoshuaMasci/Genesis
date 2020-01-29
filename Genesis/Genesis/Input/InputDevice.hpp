#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	typedef uint32_t Timestamp;

	struct ButtonValue
	{
		bool current_value = 0;
		bool prev_value = 0;
		Timestamp timestamp = 0;
	};

	struct AxisValue
	{
		AxisValue(double value = 0.0, Timestamp time = 0) :value(value), timestamp(time) {};
		double value;
		Timestamp timestamp;
	};

	//The abstract interface for an input device
	class InputDevice
	{
	public:
		InputDevice(string device_name)
			:name(device_name) {};

		string getName() { return this->name; };

		//Has a binding of that name
		virtual bool hasButton(string button_name) = 0;
		virtual ButtonValue getButton(string button_name) = 0;
		//Returns if the button is currently being pressed
		virtual bool getButtonDown(string button_name) = 0;
		//Returns true only once when the button is first pressed
		virtual bool getButtonPressed(string button_name) = 0;

		virtual bool hasAxis(string axis_name) = 0;
		virtual AxisValue getAxis(string axis_name) = 0;

		virtual void updateValues() = 0;

	protected:
		string name;
	};
}