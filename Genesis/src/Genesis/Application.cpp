#include "Application.hpp"

using namespace Genesis;

Application::Application()
	:input_manager("config/input")
{

}

Application::~Application()
{
	//Needs to wait till the GPU finishes before deleting stuff
	if (this->rendering_backend != nullptr)
	{
		this->rendering_backend->waitTillDone();
	}

	if (this->scene != nullptr)
	{
		delete this->scene;
	}

	if (this->rendering_backend != nullptr)
	{
		delete this->rendering_backend;
	}

	if (this->window != nullptr)
	{
		delete this->window;
	}

	if (this->platform != nullptr)
	{
		delete this->platform;
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

void Application::drawFrame(double delta_time)
{
	this->scene->drawFrame(delta_time);
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