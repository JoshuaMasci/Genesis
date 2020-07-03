#pragma once

#include "Genesis/Input/InputDevice.hpp"

#include "Genesis/Input/MouseDevice.hpp"
#include "Genesis/Input/KeyboardDevice.hpp"

namespace Genesis
{
	class InputManager
	{
	public:
		InputManager(string config_folder_path);
		~InputManager();

		bool getButtonDown(string name);
		bool getButtonPressed(string name);
		double getAxis(string name);

		double getButtonAxisCombo(string axis_name, string pos_button_name, string neg_button_name, bool clamp_value = true);

		void update();

		void addInputDevice(InputDevice* device);
		void removeInputDevice(InputDevice* device);

		void setMouseDevice(MouseDevice* device);
		void removeMouseDevice();

		void setKeyboardDevice(KeyboardDevice* device);
		void removeKeyboardDevice();

		//Menu Mode Mouse Functions
		void setMousePosition(vector2F position);
		vector2F getMousePosition();

		//Keyboard
		string getInputText() { return this->keyboard_device->getInputText(); };
		bool getKeyboardButtonState(KeyboardButton button) { return this->keyboard_device->getButtonState(button); };

	private:
		MouseDevice* mouse_device;
		KeyboardDevice* keyboard_device;

		flat_hash_set<InputDevice*> devices;

		//Menu Mode Mouse
		vector2F current_mouse_position;
		bool current_mouse_state[5];
	};
};