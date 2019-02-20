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

		void update();

		void addInputDevice(InputDevice* device);
		void removeInputDevice(InputDevice* device);

	private:
		set<InputDevice*> devices;
	};
};