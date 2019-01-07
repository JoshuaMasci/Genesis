#include "SDL2_Platform.hpp"

#include "SDL2_Include.hpp"

#include "Genesis/Application.hpp"

Genesis::SDL2_Platform::SDL2_Platform(Application* app)
	:Genesis::Platform(app)
{
	this->joystick_devices = new map<int32_t, SDL2_JoystickDevice*>();

	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK);
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
	//Create Keyboard Device
	
	//Create Mouse Device
	
}

Genesis::SDL2_Platform::~SDL2_Platform()
{
	//Delete Input Devices

	delete this->joystick_devices;
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
			SDL2_JoystickDevice* device = new SDL2_JoystickDevice(SDL_JoystickName(joystick), id);
			(*this->joystick_devices)[id] = device;

			//TODO load settings

			this->application->input_manager.addInputDevice(device);
		}
		else if (event.type == SDL_JOYDEVICEREMOVED)
		{
			//Destroy Joystick Device
			int id = event.jdevice.which;
			SDL2_JoystickDevice* device = (*this->joystick_devices)[id];

			this->application->input_manager.removeInputDevice(device);
			this->joystick_devices->erase(id);

			delete device;
			SDL_JoystickClose(SDL_JoystickFromInstanceID(id));
		}
	}
}
