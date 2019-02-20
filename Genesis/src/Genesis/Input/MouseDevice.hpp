#pragma once

#include "Genesis/Input/InputDevice.hpp"

namespace Genesis
{
	enum class MouseButton
	{
		Left,
		Middle,
		Right,
		Extra1,
		Extra2,
		ForwardScroll,
		BackwardScroll
	};

	enum class MouseAxis
	{
		X,
		Y
	};

	class MouseDevice : public InputDevice
	{
	public:
		MouseDevice(string name);
		~MouseDevice();

		void updateMouseButton(MouseButton button, bool state, Timestamp time);
		void updateMouseAxis(MouseAxis axis, int32_t movement_in_pixels, Timestamp time);

		virtual void updateValues();

		virtual string getAxisName(uint16_t index) override;
		virtual string getButtonName(uint16_t index) override;

	protected:
		const double pixel_divisor = 100.0;
	};
};