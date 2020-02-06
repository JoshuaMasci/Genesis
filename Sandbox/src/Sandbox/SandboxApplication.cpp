#include "SandboxApplication.hpp"

//Hardcode For now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "VulkanBackend.hpp"

SandboxApplication::SandboxApplication()
{
	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox: ");
	this->rendering_backend = new Genesis::VulkanBackend(this->window, this->job_system.getNumberOfJobThreads());
	this->render_system = new Genesis::RenderSystem(this->rendering_backend);

	this->world = new Genesis::World();
}

SandboxApplication::~SandboxApplication()
{
	delete this->world;
}

void SandboxApplication::update(Genesis::TimeStep time_step)
{
	Genesis::Application::update(time_step);

	this->world->runSimulation(this, time_step);
}

void SandboxApplication::render(Genesis::TimeStep time_step)
{
	Genesis::Application::render(time_step);
}
