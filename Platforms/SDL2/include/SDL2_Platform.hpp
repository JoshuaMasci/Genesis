#pragma once

#include "genesis_engine/platform/Platform.hpp"

#include "genesis_engine/input/keyboard_device.hpp"
#include "genesis_engine/input/mouse_device.hpp"
#include "genesis_engine/input/joystick_device.hpp"


namespace genesis
{
	class  SDL2_Platform : public Platform
	{
	public:
		SDL2_Platform(Application* app);
		virtual ~SDL2_Platform();

		virtual void onUpdate(TimeStep time_step) override;
		
	private:

		KeyboardDevice keyboard_device;
		MouseDevice mouse_device;

		//Keep track of joystick devices so they can be deleted
		flat_hash_map<int32_t, JoystickDevice*> joystick_devices;
	};
}