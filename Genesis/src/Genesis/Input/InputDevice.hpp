#pragma once

#include "Genesis/Core/DLL.hpp"
#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	class InputDevice
	{
	public:
		InputDevice(string name) { this->name = name; };

		virtual bool hasAxis(string axis_name) = 0;
		virtual double getAxis(string axis_name) = 0;

		virtual bool hasButton(string button_name) = 0;
		virtual bool getButton(string button_name) = 0;

	private:
		string name;
	};
};