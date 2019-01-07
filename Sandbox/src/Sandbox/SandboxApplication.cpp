#include "SandboxApplication.hpp"

//Hardcode For now
#include "Genesis/Platform/SDL2/SDL2_Platform.hpp" 
#include "Genesis/Platform/SDL2/SDL2_Window.hpp" 

SandboxApplication::SandboxApplication()
{
	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2I(1600, 900), "Sandbox");
}
