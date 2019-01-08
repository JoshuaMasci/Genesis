#pragma once

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/SDL2/SDL2_JoystickDevice.hpp"

namespace Genesis
{
	class  SDL2_Platform : public Genesis::Platform
	{
	public:
		SDL2_Platform(Application* app);
		virtual ~SDL2_Platform();

		virtual void onUpdate(double delta_time) override;
		
	private:

		//Keep track of joystick devices so they can be deleted
		map<int32_t, SDL2_JoystickDevice*> joystick_devices;
	};
}