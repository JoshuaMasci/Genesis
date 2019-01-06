#include "EventSystem.hpp"

Genesis::EventSystem::EventSystem()
{
	this->event_callbacks = new EVENT_CALLBACK_MAP();
}

Genesis::EventSystem::~EventSystem()
{
	delete this->event_callbacks;
}
