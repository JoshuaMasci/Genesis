#include "Application.hpp"

Genesis::Application::Application()
	:input_manager("config/input")
{

}

Genesis::Application::~Application() 
{
	if (this->platform != nullptr)
	{
		delete this->platform;
	}

	if (this->window != nullptr)
	{
		delete this->window;
	}
}

void Genesis::Application::run_tick(double delta_time)
{
	if (this->platform != nullptr)
	{
		this->platform->onUpdate(delta_time);
	}
}

void Genesis::Application::close()
{
	this->is_running = false;
	//TODO emit event
}

bool Genesis::Application::isRunning()
{ 
	return this->is_running; 
}
