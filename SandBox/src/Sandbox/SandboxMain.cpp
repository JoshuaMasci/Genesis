#pragma once

#include "Sandbox/SandboxApplication.hpp"

#include "Genesis/Platform/SDL2/SDL2_Window.hpp"

struct Event
{
	int number;
};

struct Event1
{
	bool boo = false;
};

class Test
{
public:
	void onEvent(Event* e)
	{
		printf("Event: %d\n", e->number);
	};

	void onEvent(Event1* e)
	{
		printf("Event: %d\n", e->boo);
	};
};

int main(int argc, char** argv)
{
	//ALPHA

	//Init Logging

	SandboxApplication* sandbox = new SandboxApplication();

	Test test;
	Event eve = {15};
	Event1 eve1 = { true };

	sandbox->event_system.subscribe<Event>(BindFunc(Test, Event, onEvent));
	sandbox->event_system.subscribe<Event1>(BindFunc(Test, Event1, onEvent));
	sandbox->event_system.emit<Event>(eve);
	sandbox->event_system.emit<Event1>(eve1);


	while (sandbox->isRunning())
	{
		sandbox->run_tick(0.0);
	}

	delete sandbox;

	return 0;
	//OMEGA
}