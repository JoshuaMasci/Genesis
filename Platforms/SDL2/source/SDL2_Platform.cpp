#include "SDL2_Platform.hpp"

#include "SDL2_Include.hpp"

#include "Genesis/Core/Application.hpp"

#include "Genesis/Debug/Log.hpp"

using namespace Genesis;

SDL2_Platform::SDL2_Platform(Application* app)
	:Platform(app)
{
	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK);
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

	//Create Keyboard Device
	this->keyboard_device = new KeyboardDevice("SDL2 Keyboard");
	this->application->input_manager.setKeyboardDevice(this->keyboard_device);

	this->keyboard_device->addButton("Debug_Forward", (ButtonIndex) KeyboardButton::W);
	this->keyboard_device->addButton("Debug_Backward", (ButtonIndex)KeyboardButton::S);
	this->keyboard_device->addButton("Debug_Up", (ButtonIndex)KeyboardButton::LShift);
	this->keyboard_device->addButton("Debug_Down", (ButtonIndex)KeyboardButton::LCtrl);
	this->keyboard_device->addButton("Debug_Left", (ButtonIndex)KeyboardButton::A);
	this->keyboard_device->addButton("Debug_Right", (ButtonIndex)KeyboardButton::D);

	this->keyboard_device->addButton("Debug_PitchUp", (ButtonIndex)KeyboardButton::Up);
	this->keyboard_device->addButton("Debug_PitchDown", (ButtonIndex)KeyboardButton::Down);
	this->keyboard_device->addButton("Debug_YawLeft", (ButtonIndex)KeyboardButton::Left);
	this->keyboard_device->addButton("Debug_YawRight", (ButtonIndex)KeyboardButton::Right);
	this->keyboard_device->addButton("Debug_RollLeft", (ButtonIndex)KeyboardButton::Q);
	this->keyboard_device->addButton("Debug_RollRight", (ButtonIndex)KeyboardButton::E);

	//Create Mouse Device
	this->mouse_device = new MouseDevice("SDL2 Mouse");
	this->application->input_manager.setMouseDevice(this->mouse_device);

	this->mouse_device->addButton("Mouse_Left", (ButtonIndex)MouseButton::Left);
	this->mouse_device->addButton("Mouse_Middle", (ButtonIndex)MouseButton::Middle);
	this->mouse_device->addButton("Mouse_Right", (ButtonIndex)MouseButton::Right);
	this->mouse_device->addButton("Mouse_X1", (ButtonIndex)MouseButton::Extra1);
	this->mouse_device->addButton("Mouse_X2", (ButtonIndex)MouseButton::Extra2);
	this->mouse_device->addButton("Mouse_ForwardScroll", (ButtonIndex)MouseButton::ForwardScroll);
	this->mouse_device->addButton("Mouse_BackwardScroll", (ButtonIndex)MouseButton::BackwardScroll);
}

SDL2_Platform::~SDL2_Platform()
{
	if (this->keyboard_device != nullptr)
	{
		this->application->input_manager.removeKeyboardDevice();
		delete this->keyboard_device;
	}

	if (this->mouse_device != nullptr)
	{
		this->application->input_manager.removeMouseDevice();
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

MouseButton getGenesisMouseButton(Uint8 button)
{
	switch (button)
	{
	case SDL_BUTTON_LEFT:
		return MouseButton::Left;
	case SDL_BUTTON_MIDDLE:
		return MouseButton::Middle;
	case SDL_BUTTON_RIGHT:
		return MouseButton::Right;
	case SDL_BUTTON_X1:
		return MouseButton::Extra1;
	case SDL_BUTTON_X2:
		return MouseButton::Extra2;
	default:
		return (MouseButton)65535;//Return the largest possible value, should error out somewhere
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

KeyboardButton getGenesisKeyboardButton(SDL_Scancode scancode)
{
	switch (scancode)
	{
	case SDL_SCANCODE_A:
		return KeyboardButton::A;
	case SDL_SCANCODE_B:
		return KeyboardButton::B;
	case SDL_SCANCODE_C:
		return KeyboardButton::C;
	case SDL_SCANCODE_D:
		return KeyboardButton::D;
	case SDL_SCANCODE_E:
		return KeyboardButton::E;
	case SDL_SCANCODE_F:
		return KeyboardButton::F;
	case SDL_SCANCODE_G:
		return KeyboardButton::G;
	case SDL_SCANCODE_H:
		return KeyboardButton::H;
	case SDL_SCANCODE_I:
		return KeyboardButton::I;
	case SDL_SCANCODE_J:
		return KeyboardButton::J;
	case SDL_SCANCODE_K:
		return KeyboardButton::K;
	case SDL_SCANCODE_L:
		return KeyboardButton::L;
	case SDL_SCANCODE_M:
		return KeyboardButton::M;
	case SDL_SCANCODE_N:
		return KeyboardButton::N;
	case SDL_SCANCODE_O:
		return KeyboardButton::O;
	case SDL_SCANCODE_P:
		return KeyboardButton::P;
	case SDL_SCANCODE_Q:
		return KeyboardButton::Q;
	case SDL_SCANCODE_R:
		return KeyboardButton::R;
	case SDL_SCANCODE_S:
		return KeyboardButton::S;
	case SDL_SCANCODE_T:
		return KeyboardButton::T;
	case SDL_SCANCODE_U:
		return KeyboardButton::U;
	case SDL_SCANCODE_V:
		return KeyboardButton::V;
	case SDL_SCANCODE_W:
		return KeyboardButton::W;
	case SDL_SCANCODE_X:
		return KeyboardButton::X;
	case SDL_SCANCODE_Y:
		return KeyboardButton::Y;
	case SDL_SCANCODE_Z:
		return KeyboardButton::Z;
	case SDL_SCANCODE_1:
		return KeyboardButton::Key_1;
	case SDL_SCANCODE_2:
		return KeyboardButton::Key_2;
	case SDL_SCANCODE_3:
		return KeyboardButton::Key_3;
	case SDL_SCANCODE_4:
		return KeyboardButton::Key_4;
	case SDL_SCANCODE_5:
		return KeyboardButton::Key_5;
	case SDL_SCANCODE_6:
		return KeyboardButton::Key_6;
	case SDL_SCANCODE_7:
		return KeyboardButton::Key_7;
	case SDL_SCANCODE_8:
		return KeyboardButton::Key_8;
	case SDL_SCANCODE_9:
		return KeyboardButton::Key_9;
	case SDL_SCANCODE_0:
		return KeyboardButton::Key_0;
	case SDL_SCANCODE_LSHIFT:
		return KeyboardButton::LShift;
	case SDL_SCANCODE_LCTRL:
		return KeyboardButton::LCtrl;
	case SDL_SCANCODE_LALT:
		return KeyboardButton::LAlt;
	case SDL_SCANCODE_RSHIFT:
		return KeyboardButton::RShift;
	case SDL_SCANCODE_RCTRL:
		return KeyboardButton::RCtrl;
	case SDL_SCANCODE_RALT:
		return KeyboardButton::RAlt;
	case SDL_SCANCODE_RETURN:
		return KeyboardButton::Enter;
	case SDL_SCANCODE_BACKSPACE:
		return KeyboardButton::Backspace;
	case SDL_SCANCODE_TAB:
		return KeyboardButton::Tab;
	case SDL_SCANCODE_SPACE:
		return KeyboardButton::Space;
	case SDL_SCANCODE_MINUS:
		return KeyboardButton::Minus;
	case SDL_SCANCODE_EQUALS:
		return KeyboardButton::Equals;
	case SDL_SCANCODE_LEFTBRACKET:
		return KeyboardButton::LBracket;
	case SDL_SCANCODE_RIGHTBRACKET:
		return KeyboardButton::RBracket;
	case SDL_SCANCODE_BACKSLASH:
		return KeyboardButton::Backslash;
	case SDL_SCANCODE_SEMICOLON:
		return KeyboardButton::Semicolon;
	case SDL_SCANCODE_APOSTROPHE:
		return KeyboardButton::Apostrophe;
	case SDL_SCANCODE_COMMA:
		return KeyboardButton::Comma;
	case SDL_SCANCODE_PERIOD:
		return KeyboardButton::Period;
	case SDL_SCANCODE_SLASH:
		return KeyboardButton::Slash;
	case SDL_SCANCODE_CAPSLOCK:
		return KeyboardButton::Capslock;
	case SDL_SCANCODE_F1:
		return KeyboardButton::F1;
	case SDL_SCANCODE_F2:
		return KeyboardButton::F2;
	case SDL_SCANCODE_F3:
		return KeyboardButton::F3;
	case SDL_SCANCODE_F4:
		return KeyboardButton::F4;
	case SDL_SCANCODE_F5:
		return KeyboardButton::F5;
	case SDL_SCANCODE_F6:
		return KeyboardButton::F6;
	case SDL_SCANCODE_F7:
		return KeyboardButton::F7;
	case SDL_SCANCODE_F8:
		return KeyboardButton::F8;
	case SDL_SCANCODE_F9:
		return KeyboardButton::F9;
	case SDL_SCANCODE_F10:
		return KeyboardButton::F10;
	case SDL_SCANCODE_F11:
		return KeyboardButton::F11;
	case SDL_SCANCODE_F12:
		return KeyboardButton::F12;
	case SDL_SCANCODE_PRINTSCREEN:
		return KeyboardButton::Printscreen;
	case SDL_SCANCODE_SCROLLLOCK:
		return KeyboardButton::Scrolllock;
	case SDL_SCANCODE_PAUSE:
		return KeyboardButton::Pause;
	case SDL_SCANCODE_INSERT:
		return KeyboardButton::Insert;
	case SDL_SCANCODE_HOME:
		return KeyboardButton::Home;
	case SDL_SCANCODE_PAGEUP:
		return KeyboardButton::Pageup;
	case SDL_SCANCODE_DELETE:
		return KeyboardButton::Delete;
	case SDL_SCANCODE_END:
		return KeyboardButton::End;
	case SDL_SCANCODE_PAGEDOWN:
		return KeyboardButton::Pagedown;
	case SDL_SCANCODE_RIGHT:
		return KeyboardButton::Right;
	case SDL_SCANCODE_LEFT:
		return KeyboardButton::Left;
	case SDL_SCANCODE_DOWN:
		return KeyboardButton::Down;
	case SDL_SCANCODE_UP:
		return KeyboardButton::Up;
	default:
		return KeyboardButton::SIZE; //Use size as an invalid value;
	}
}

void SDL2_Platform::onUpdate(TimeStep time_step)
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
		//Keyboard
		else if (event.type == SDL_KEYDOWN)
		{
			if (this->keyboard_device != nullptr)
			{
				this->keyboard_device->updateKeyboardButton(getGenesisKeyboardButton(event.key.keysym.scancode), true, event.key.timestamp);
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			if (this->keyboard_device != nullptr)
			{
				this->keyboard_device->updateKeyboardButton(getGenesisKeyboardButton(event.key.keysym.scancode), false, event.key.timestamp);
			}
		}
		else if (event.type == SDL_TEXTINPUT)
		{
			if (this->keyboard_device != nullptr)
			{
				this->keyboard_device->updateInputText(event.text.text);
			}
		}

		//MOUSE
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (this->mouse_device != nullptr)
			{
				this->mouse_device->updateMouseButton(getGenesisMouseButton(event.button.button), true, event.button.timestamp);
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (this->mouse_device != nullptr)
			{
				this->mouse_device->updateMouseButton(getGenesisMouseButton(event.button.button), false, event.button.timestamp);
			}
		}
		else if (event.type == SDL_MOUSEWHEEL)
		{
			if (this->mouse_device != nullptr)
			{
				if (event.wheel.y > 0) // scroll up
				{
					this->mouse_device->updateMouseButton(MouseButton::ForwardScroll, true, event.button.timestamp);
				}
				else if (event.wheel.y < 0) // scroll down
				{
					this->mouse_device->updateMouseButton(MouseButton::BackwardScroll, true, event.button.timestamp);
				}
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			if (this->mouse_device != nullptr)
			{
				//Only works when mouse is captued and is not in menu mode
				if (true)
				{
					//Capture Mode
					this->mouse_device->updateMouseAxis(MouseAxis::X, event.motion.xrel, event.motion.timestamp);
					this->mouse_device->updateMouseAxis(MouseAxis::Y, event.motion.yrel, event.motion.timestamp);

					//Menu Mode
					vector2F pos = { (float)event.motion.x , (float)event.motion.y };
					this->application->input_manager.setMousePosition(pos);
				}
			}
		}

		//Joystick
		else if (event.type == SDL_JOYDEVICEADDED)
		{
			//Create new Joystick Device
			int32_t id = event.jdevice.which;
			SDL_Joystick* joystick = SDL_JoystickOpen(id);
			JoystickDevice* device = new JoystickDevice(SDL_JoystickName(joystick), SDL_JoystickNumButtons(joystick), SDL_JoystickNumHats(joystick), SDL_JoystickNumAxes(joystick));
			this->joystick_devices[id] = device;

			GENESIS_ENGINE_INFO("SDL Joystick Added: {}", device->getName());

			//Settings
			if (true)
			{
				device->addAxis("Debug_Pitch", AxisSettings(3, 0.1, false));
				device->addAxis("Debug_Yaw", AxisSettings(2, 0.1, true));

				device->addButton("Debug_RollLeft", (ButtonIndex)10);
				device->addButton("Debug_RollRight", (ButtonIndex)9);

				device->addAxis("Debug_ForwardBackward", AxisSettings(1, 0.1, true));
				device->addAxis("Debug_LeftRight", AxisSettings(0, 0.1, true));
			}


			this->application->input_manager.addInputDevice(device);
		}
		else if (event.type == SDL_JOYDEVICEREMOVED)
		{

			//Destroy Joystick Device
			int32_t id = event.jdevice.which;
			JoystickDevice* device = this->joystick_devices[id];

			GENESIS_ENGINE_INFO("SDL Joystick Removed: {}", device->getName());

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
	}
}
