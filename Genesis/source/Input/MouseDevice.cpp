#include "Genesis/Input/MouseDevice.hpp"

namespace Genesis
{
	MouseDevice::MouseDevice(string name)
		:ArrayInputDevice(name, 7, 2)
	{

	}

	MouseDevice::~MouseDevice()
	{

	}

	//A wrapper for updateButton
	void MouseDevice::updateMouseButton(MouseButton button, bool state, Timestamp time)
	{
		this->updateButton((int32_t)button, state, time);
	}

	//A wrapper for updateAxis
	void MouseDevice::updateMouseAxis(MouseAxis axis, int32_t movement_in_pixels, Timestamp time)
	{
		double value = ((double)movement_in_pixels) / pixel_divisor;
		this->updateAxis((int16_t)axis, value, time);
	}

	void MouseDevice::updateValues()
	{
		ArrayInputDevice::updateValues();//Call super

		//Always clear the scroll wheel
		this->updateButton((int16_t)MouseButton::ForwardScroll, false, 0);//Update time doesnt matter
		this->updateButton((int16_t)MouseButton::BackwardScroll, false, 0);

		//Clear mouse movements
		this->updateAxis((int16_t)MouseAxis::X, 0.0, 0);//Clear time so that it doesn't get used
		this->updateAxis((int16_t)MouseAxis::Y, 0.0, 0);
	}

	string MouseDevice::getAxisName(uint16_t index)
	{
		return string();
	}

	string MouseDevice::getButtonName(uint16_t index)
	{
		return string();
	}
}