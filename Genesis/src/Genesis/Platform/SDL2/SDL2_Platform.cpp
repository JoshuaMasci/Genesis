#include "SDL2_Platform.hpp"

#include "SDL2_Include.hpp"

#include "Genesis/Application.hpp"

Genesis::SDL2_Platform::SDL2_Platform(Application* app)
	:Genesis::Platform(app)
{
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK);
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
	//Create Keyboard Device
	
	//Create Mouse Device
	
}

Genesis::SDL2_Platform::~SDL2_Platform()
{
	//Delete Input Devices
	for (auto device : this->joystick_devices)
	{
		this->application->input_manager.removeInputDevice(device.second);
		delete device.second;
		SDL_JoystickClose(SDL_JoystickFromInstanceID(device.first));
	}

	SDL_Quit();
}

void Genesis::SDL2_Platform::onUpdate(double delta_time)
{
	//Event Loop
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
		{
			//Close Application
			this->application->close();
		}
		else if (event.type == SDL_JOYDEVICEADDED)
		{
			//Create new Joystick Device
			int32_t id = event.jdevice.which;
			SDL_Joystick* joystick = SDL_JoystickOpen(id);
			InputDevice* device = new InputDevice(SDL_JoystickName(joystick), SDL_JoystickNumButtons(joystick), SDL_JoystickNumAxes(joystick));
			this->joystick_devices[id] = device;

			//Settings

			this->application->input_manager.addInputDevice(device);
		}
		else if (event.type == SDL_JOYDEVICEREMOVED)
		{
			//Destroy Joystick Device
			int32_t id = event.jdevice.which;
			InputDevice* device = this->joystick_devices[id];

			this->application->input_manager.removeInputDevice(device);
			this->joystick_devices.erase(id);

			delete device;
			SDL_JoystickClose(SDL_JoystickFromInstanceID(id));
		}
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			int32_t id = event.jbutton.which;
			InputDevice* device = this->joystick_devices[id];
			device->updateButton(event.jbutton.button, true, event.jbutton.timestamp);
		}
		else if (event.type == SDL_JOYBUTTONUP)
		{
			int32_t id = event.jbutton.which;
			InputDevice* device = this->joystick_devices[id];
			device->updateButton(event.jbutton.button, false, event.jbutton.timestamp);
		}
		else if (event.type == SDL_JOYAXISMOTION)
		{
			int32_t id = event.jaxis.which;
			InputDevice* device = this->joystick_devices[id];
			device->updateAxis(event.jaxis.axis, ((double)event.jaxis.value) / 32767.0, event.jaxis.timestamp);
		}
	}
}
