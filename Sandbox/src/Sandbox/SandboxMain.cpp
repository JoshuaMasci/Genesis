#pragma once

#include "Sandbox/SandboxApplication.hpp"

int main(int argc, char** argv)
{
	//ALPHA
	SandboxApplication* sandbox = new SandboxApplication();

	//sandbox->event_system.subscribe<Event>(BindFunc(Test, Event, onEvent));

	while (sandbox->isRunning())
	{
		sandbox->run_tick(0.0);
	}

	delete sandbox;

	return 0;
	//OMEGA
}