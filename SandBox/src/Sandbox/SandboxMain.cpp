#pragma once

#include "Sandbox/SandboxApplication.hpp"

#include "Genesis/Platform/SDL2/SDL2_Window.hpp"

int main(int argc, char** argv)
{
	//ALPHA

	//Init Logging

	//Create Platform

	//Create Window
	Genesis::SDL2_Window* window = new Genesis::SDL2_Window(Genesis::vector2I(1920, 1080), "Sandbox");

	//Create Input system

	SandboxApplication* sandbox = new SandboxApplication();

	while (sandbox->isRunning())
	{
		sandbox->run_tick(0.0);
	}

	delete window;
	delete sandbox;

	return 0;
	//OMEGA
}