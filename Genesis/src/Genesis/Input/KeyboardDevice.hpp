#pragma once

#include "Genesis/Input/InputDevice.hpp"

namespace Genesis
{
	class KeyboardDevice : public InputDevice
	{
	public:
		KeyboardDevice(string name);
		~KeyboardDevice();


		virtual void updateValues();

		virtual string getAxisName(uint16_t index) override;
		virtual string getButtonName(uint16_t index) override;

	protected:
	};
};