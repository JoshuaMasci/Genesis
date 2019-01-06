#pragma once

#include "Genesis/Core/DLL.hpp"
#include "Genesis/Core/Types.hpp"
Window* createWindow(void* window_settings);

namespace Genesis
{
	//Abstract platform class
	class GENESIS_DLL Platform
	{
	public:
		virtual void onUpdate(double delta_time) = 0;
		
		//Create a window to be passed off to the application
		//Platforms can choose to save a copy of the window to send events to
		Window* createWindow(void* window_settings);
	};
};