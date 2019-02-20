#include "SDL2_Platform.hpp"

#include "SDL2_Include.hpp"

#include "Genesis/Application.hpp"

using namespace Genesis;

SDL2_Platform::SDL2_Platform(Application* app)
	:Platform(app)
{
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK);
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
	//Create Keyboard Device
	
	//Create Mouse Device
	this->mouse_device = new InputDevice("MouseDevice", 7, 2);
	this->application->input_manager.addInputDevice(this->mouse_device);
}

SDL2_Platform::~SDL2_Platform()
{
	if (this->keyboard_device != nullptr)
	{
		this->application->input_manager.removeInputDevice(this->keyboard_device);
		delete this->keyboard_device;
	}

	if (this->mouse_device != nullptr)
	{
		this->application->input_manager.removeInputDevice(this->mouse_device);
		delete this->mouse_device;
	}

	//Delete Input Devices
	for (auto device : this->joystick_devices)
	{
		this->application->input_manager.removeInputDevice(device.second);
		delete device.second;
		SDL_JoystickClose(SDL_JoystickFromInstanceID(device.first));
	}

	SDL_Quit();
}

uint16_t getGenesisMouseButton(Uint8 button)
{
	switch (button)
	{
	case SDL_BUTTON_LEFT:
		return (uint16_t)Mouse::MouseButtons::Left;
	case SDL_BUTTON_MIDDLE:
		return (uint16_t)Mouse::MouseButtons::Middle;
	case SDL_BUTTON_RIGHT:
		return (uint16_t)Mouse::MouseButtons::Right;
	case SDL_BUTTON_X1:
		return (uint16_t)Mouse::MouseButtons::Extra1;
	case SDL_BUTTON_X2:
		return (uint16_t)Mouse::MouseButtons::Extra2;
	default:
		return 65535;//Return the largest possible value, should error out somewhere
	}
}

HatState getGenesisHatState(uint8_t state)
{
	switch (state)
	{
	case SDL_HAT_UP:
		return HatState::Up;
	case SDL_HAT_RIGHT:
		return HatState::Right;
	case SDL_HAT_DOWN:
		return HatState::Down;
	case SDL_HAT_LEFT:
		return HatState::Left;
	case SDL_HAT_RIGHTUP:
		return HatState::RightUp;
	case SDL_HAT_RIGHTDOWN:
		return HatState::RightDown;
	case SDL_HAT_LEFTUP:
		return HatState::LeftUp;
	case SDL_HAT_LEFTDOWN:
		return HatState::LeftDown;
	default:
		return HatState::Centered;//SDL_HAT_CENTERED and error cases
	}
}

void SDL2_Platform::onUpdate(double delta_time)
{
	//Clear Scroll Wheel
	this->mouse_device->updateButton((int32_t)Mouse::MouseButtons::ForwardScroll, false, SDL_GetTicks());
	this->mouse_device->updateButton((int32_t)Mouse::MouseButtons::BackwardScroll, false, SDL_GetTicks());

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
			JoystickDevice* device = new JoystickDevice(SDL_JoystickName(joystick), SDL_JoystickNumButtons(joystick), SDL_JoystickNumHats(joystick), SDL_JoystickNumAxes(joystick));
			this->joystick_devices[id] = device;

			//Settings

			this->application->input_manager.addInputDevice(device);
		}
		else if (event.type == SDL_JOYDEVICEREMOVED)
		{
			//Destroy Joystick Device
			int32_t id = event.jdevice.which;
			JoystickDevice* device = this->joystick_devices[id];

			this->application->input_manager.removeInputDevice(device);
			this->joystick_devices.erase(id);

			delete device;
			SDL_JoystickClose(SDL_JoystickFromInstanceID(id));
		}
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			int32_t id = event.jbutton.which;
			JoystickDevice* device = this->joystick_devices[id];
			device->updateButton(event.jbutton.button, true, event.jbutton.timestamp);
		}
		else if (event.type == SDL_JOYBUTTONUP)
		{
			int32_t id = event.jbutton.which;
			JoystickDevice* device = this->joystick_devices[id];
			device->updateButton(event.jbutton.button, false, event.jbutton.timestamp);
		}
		else if (event.type == SDL_JOYAXISMOTION)
		{
			int32_t id = event.jaxis.which;
			JoystickDevice* device = this->joystick_devices[id];
			device->updateAxis(event.jaxis.axis, ((double)event.jaxis.value) / 32767.0, event.jaxis.timestamp);
		}
		else if (event.type == SDL_JOYHATMOTION)
		{
			int32_t id = event.jhat.which;
			JoystickDevice* device = this->joystick_devices[id];
			int32_t hat = event.jhat.hat;
			device->updateHat(hat, getGenesisHatState(event.jhat.value), event.jhat.timestamp);
		}
		//MOUSE
		else if(event.type == SDL_MOUSEBUTTONDOWN)
		{
			this->mouse_device->updateButton(getGenesisMouseButton(event.button.button), true, event.button.timestamp);
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			this->mouse_device->updateButton(getGenesisMouseButton(event.button.button), false, event.button.timestamp);
		}
		else if (event.type == SDL_MOUSEWHEEL)
		{
			if (event.wheel.y > 0) // scroll up
			{
				this->mouse_device->updateButton((uint16_t)Mouse::MouseButtons::ForwardScroll, true, event.button.timestamp);
			}
			else if (event.wheel.y < 0) // scroll down
			{
				this->mouse_device->updateButton((uint16_t)Mouse::MouseButtons::BackwardScroll, true, event.button.timestamp);
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			//Only works when mouse is captued and is not in menu mode

		}
	}
}
