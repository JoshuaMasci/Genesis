#pragma once

#include "Genesis/Core/DLL.hpp"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	struct AxisReturn
	{
		double value;
		int timestamp;
	};
	
	class GENESIS_DLL InputDevice
	{
	public:
		InputDevice(string name) {};
		InputDevice(string name, void* settings) {}
		
		virtual bool hasAxis(string axis_name) = 0;
		virtual double getAxis(string axis_name) = 0;

		virtual bool hasButton(string button_name) = 0;
		virtual bool getButton(string button_name) = 0;
		
		string getName() { return ""; };

	private:
	};
};