#include "SandboxApplication.hpp"

//Hardcode For now
#include "Genesis/Platform/SDL2/SDL2_Platform.hpp" 
#include "Genesis/Platform/SDL2/SDL2_Window.hpp" 

SandboxApplication::SandboxApplication()
{
	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox Vulkan");

	this->renderer = new Genesis::VulkanRenderer(this->window, "Sandbox");
}

SandboxApplication::~SandboxApplication()
{
	delete this->renderer;
}

void SandboxApplication::runFrame(double delta_time)
{
	Genesis::Application::runFrame(delta_time);
}
