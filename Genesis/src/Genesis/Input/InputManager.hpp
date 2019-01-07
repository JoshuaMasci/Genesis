#pragma once

#include "Genesis/Core/DLL.hpp"
#include "Genesis/Core/Types.hpp"
#include "Genesis/Input/InputDevice.hpp"

namespace Genesis
{
	class GENESIS_DLL InputManager
	{
	public:
		InputManager(string config_folder_path) {};
		~InputManager() {};

		void addInputDevice(InputDevice* device) {};
		void removeInputDevice(InputDevice* device) {};

	private:
	};
};