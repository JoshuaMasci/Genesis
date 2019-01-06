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
#include <functional>
using namespace std::placeholders;

int main(int argc, char** argv)
{
	//ALPHA

	//Init Logging

	//Create Platform

	//Create Window
	Genesis::SDL2_Window* window = new Genesis::SDL2_Window(Genesis::vector2I(1600, 900), "Sandbox");

	//Create Input system

	SandboxApplication* sandbox = new SandboxApplication();

	Test test;
	Event eve = {15};
	Event1 eve1 = { true };

	sandbox->event_system.subscribe<Event>(std::bind((void(Test::*)(Event*))&Test::onEvent, &test, std::placeholders::_1));
	sandbox->event_system.subscribe<Event1>(std::bind((void(Test::*)(Event1*))&Test::onEvent, &test, std::placeholders::_1));
	sandbox->event_system.emit<Event>(eve);
	eve.number = 128;
	sandbox->event_system.emit<Event>(eve);
	sandbox->event_system.emit<Event1>(eve1);


	while (sandbox->isRunning())
	{
		sandbox->run_tick(0.0);
	}

	delete window;
	delete sandbox;

	return 0;
	//OMEGA
}