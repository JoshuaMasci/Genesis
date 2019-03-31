#include "Application.hpp"

using namespace Genesis;

Application::Application()
	:input_manager("config/input")
{

}

Application::~Application()
{
	if (this->window != nullptr)
	{
		delete this->window;
	}

	if (this->platform != nullptr)
	{
		delete this->platform;
	}

	if (this->scene != nullptr)
	{
		delete this->scene;
	}
}

void Application::runSimulation(double delta_time)
{
	this->input_manager.update();

	if (this->platform != nullptr)
	{
		this->platform->onUpdate(delta_time);
	}

	if (this->scene != nullptr)
	{
		this->scene->runSimulation(delta_time);
	}
}

void Application::runFrame(double delta_time)
{

}

void Application::close()
{
	this->is_running = false;
	//TODO emit event
}

bool Application::isRunning()
{
	return this->is_running;
}