#include "SDL2_Window.hpp"
#include "SDL2_Include.hpp"

#include <SDL_syswm.h>

namespace Genesis
{
	SDL2_Window::SDL2_Window(vector2U size, string title, bool use_opengl)
		:Window(size, title)
	{
		SDL_Init(SDL_INIT_VIDEO);

		uint32_t flags = 0;

		if (use_opengl)
		{
			flags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL;
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		}

		this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, flags);
		SDL_RaiseWindow(this->window);
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

	void* SDL2_Window::getNativeWindowHandle()
	{
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(window, &info);

		return (void*)info.info.win.window;
	}

	void* SDL2_Window::GL_CreateContext()
	{
		return (void*)SDL_GL_CreateContext(this->window);
	}

	void SDL2_Window::GL_SetVsync(Vsync setting)
	{
		SDL_GL_SetSwapInterval((int)setting);
	}

	Vsync SDL2_Window::GL_GetVsync()
	{
		return (Vsync)SDL_GL_GetSwapInterval();
	}

	void SDL2_Window::GL_UpdateBuffer()
	{
		SDL_GL_SwapWindow(this->window);
	}

	void SDL2_Window::GL_DeleteContext(void* context)
	{
		SDL_GL_DeleteContext((SDL_GLContext)context);
	}
}