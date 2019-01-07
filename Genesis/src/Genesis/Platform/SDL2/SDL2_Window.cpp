#include "SDL2_Window.hpp"
#include "Genesis/Platform/SDL2/SDL2_Include.hpp"

using namespace Genesis;

SDL2_Window::SDL2_Window(vector2I size, string title)
	:Window(size, title)
{
	SDL_Init(SDL_INIT_VIDEO);
	this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, SDL_WINDOW_RESIZABLE);
}

SDL2_Window::~SDL2_Window()
{
	SDL_DestroyWindow(this->window);
	this->window = nullptr;
}

vector2I SDL2_Window::getWindowSize()
{
	vector2I size;
	SDL_GetWindowSize(window, &size.x, &size.y);
	return size;
}

void SDL2_Window::setWindowSize(vector2I size)
{
	SDL_SetWindowSize(window, size.x, size.y);
}

void SDL2_Window::setWindowTitle(string title)
{
	SDL_SetWindowTitle(window, title.c_str());
}

void Genesis::SDL2_Window::setVsync(VSYNC setting)
{
	SDL_GL_SetSwapInterval(setting);//Might work for OPENGL only
}

VSYNC Genesis::SDL2_Window::getVsync()
{
	return (VSYNC)SDL_GL_GetSwapInterval();
}
