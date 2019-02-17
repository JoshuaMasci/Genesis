#include "Application.hpp"

using namespace Genesis;

Application::Application()
	:input_manager("config/input")
{

}

Application::~Application() 
{
	if (this->world != nullptr)
	{
		delete this->world;
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

void Application::runFrame(double delta_time)
{
	this->input_manager.update();

	if (this->platform != nullptr)
	{
		this->platform->onUpdate(delta_time);
	}

	if (this->world != nullptr)
	{
		this->world->updatePreFrame(delta_time);

		//TODO Physics



		this->world->updatePostFrame(delta_time);
	}

	//TODO: RENDER HERE

	if (this->window != nullptr)
	{
		this->window->updateBuffer();
	}
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
