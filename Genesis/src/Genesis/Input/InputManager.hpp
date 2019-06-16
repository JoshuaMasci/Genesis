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

	private:
		set<InputDevice*> devices;
	};
};