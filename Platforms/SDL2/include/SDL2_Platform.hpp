#pragma once

#include "genesis_engine/platform/Platform.hpp"

#include "genesis_engine/input/KeyboardDevice.hpp"
#include "genesis_engine/input/MouseDevice.hpp"
#include "genesis_engine/input/JoystickDevice.hpp"


namespace Genesis
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