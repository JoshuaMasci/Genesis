#include "SDL2_Platform.hpp"

#include "SDL2_Include.hpp"

Genesis::SDL2_Platform::SDL2_Platform()
{
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK);
	
	//Create Keyboard Device
	
	//Create Mouse Device
	
}

Genesis::SDL2_Platform::~SDL2_Platform()
{
	//Delete Input Devices
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
		}
	}
}
