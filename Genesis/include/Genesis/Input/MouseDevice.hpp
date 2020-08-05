#pragma once

#include "Genesis/Input/ArrayInputDevice.hpp"

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
		BackwardScroll,
		SIZE
	};

	enum class MouseAxis
	{
		X,
		Y
	};

	class MouseDevice : public ArrayInputDevice
	{
	public:
		MouseDevice(const string& device_name, InputManager* manager);

		void addMouseBinding(MouseButton button, fnv_hash32 string_hash);
		void removeMouseBinding(MouseButton button, fnv_hash32 string_hash);

		void updateMouseState(MouseButton button, bool state);
		void updateMousePosition(const vector2F& position);
	};
}