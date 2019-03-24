#include "SDL2_Window.hpp"
#include "Genesis/Platform/SDL2/SDL2_Include.hpp"

#include <SDL_syswm.h>

using namespace Genesis;

SDL2_Window::SDL2_Window(vector2U size, string title)
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

vector2U SDL2_Window::getWindowSize()
{
	vector2U size;
	SDL_GetWindowSize(window, (int*)&size.x, (int*)&size.y);
	return size;
}

void SDL2_Window::setWindowSize(vector2U size)
{
	SDL_SetWindowSize(window, size.x, size.y);
}

void SDL2_Window::setWindowTitle(string title)
{
	SDL_SetWindowTitle(window, title.c_str());
}

void SDL2_Window::updateBuffer()
{
	SDL_GL_SwapWindow(this->window);
}

void* SDL2_Window::getNativeWindowHandle()
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);

	return (void*)info.info.win.window;
}
