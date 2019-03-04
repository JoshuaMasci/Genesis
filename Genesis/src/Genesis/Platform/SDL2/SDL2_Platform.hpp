#pragma once

#include "Genesis/Platform/Platform.hpp"

#include "Genesis/Input/KeyboardDevice.hpp"
#include "Genesis/Input/MouseDevice.hpp"
#include "Genesis/Input/JoystickDevice.hpp"


namespace Genesis
{
	class  SDL2_Platform : public Platform
	{
	public:
		SDL2_Platform(Application* app);
		virtual ~SDL2_Platform();

		virtual void onUpdate(double delta_time) override;
		
	private:

		KeyboardDevice* keyboard_device = nullptr;
		MouseDevice* mouse_device = nullptr;

		//Keep track of joystick devices so they can be deleted
		map<int32_t, JoystickDevice*> joystick_devices;
	};
}