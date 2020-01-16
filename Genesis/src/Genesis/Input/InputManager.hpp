#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Input/InputDevice.hpp"

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


		//Menu Mode Mouse Functions
		void setMousePosition(vector2F position);
		vector2F getMousePosition();

	private:
		set<InputDevice*> devices;

		//Menu Mode Mouse
		vector2F current_mouse_position;
		bool current_mouse_state[5];
	};
};