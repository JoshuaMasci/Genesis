#include <stdio.h>

#include "Genesis/Core/Transform.hpp"
#include "Genesis/Platform/SDL2/SDL2_Window.hpp"

int main()
{
	Genesis::SDL2_Window* window = new Genesis::SDL2_Window(Genesis::vector2I(1920, 1080), "Genesis Engine");

	delete window;
	return 0;
}