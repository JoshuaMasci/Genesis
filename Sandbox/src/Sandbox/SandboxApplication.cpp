#include "SandboxApplication.hpp"

//Hardcode For now
#include "Genesis/Platform/SDL2/SDL2_Platform.hpp" 
#include "Genesis/Platform/SDL2/SDL2_Window.hpp" 
#include "Genesis/Rendering/Vulkan/VulkanBackend.hpp"

SandboxApplication::SandboxApplication()
{
	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(2560, 1440), "Sandbox: ");
	this->rendering_backend = new Genesis::VulkanBackend(this->window, this->job_system.getNumberOfJobThreads());

	this->scene = new Genesis::GameScene(this);
}

SandboxApplication::~SandboxApplication()
{
}

void SandboxApplication::drawFrame(double delta_time)
{
	Genesis::Application::drawFrame(delta_time);
}
