#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	struct AxisReturn
	{
		double value;
		int timestamp;
	};
	
	class InputDevice
	{
	public:
		InputDevice(string name):name(name) {};
		InputDevice(string name, void* settings):name(name) {}
		
		virtual bool hasAxis(string axis_name) = 0;
		virtual double getAxis(string axis_name) = 0;

		virtual bool hasButton(string button_name) = 0;
		virtual bool getButton(string button_name) = 0;
		
		string getName() { return ""; };

	private:
		string name;
	};
};