#include "SandboxApplication.hpp"

//Hardcode For now
#include "Genesis/Platform/SDL2/SDL2_Platform.hpp" 
#include "Genesis/Platform/SDL2/SDL2_Window.hpp" 
#include "Genesis/Graphics/Vulkan/VulkanContext.hpp"

SandboxApplication::SandboxApplication()
{
	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(2560, 1440), "Sandbox", Genesis::GraphicsAPI::OpenGL);

	this->scene = new Genesis::GameScene(this);

	this->graphics_context = new Genesis::VulkanContext(this->window);
}

SandboxApplication::~SandboxApplication()
{
}

void SandboxApplication::drawFrame(double delta_time)
{
	Genesis::Application::drawFrame(delta_time);
}
