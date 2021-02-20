#pragma once

#include "genesis_engine/platform/Window.hpp"

//Prototype struct
struct SDL_Window;

namespace genesis_engine
{
	class SDL2_Window: public Window
	{
	public:
		SDL2_Window(vec2u size, string title, bool use_opengl = true);
		virtual ~SDL2_Window();

		virtual vec2u getWindowSize();
		virtual void setWindowSize(vec2u size);

		virtual void setWindowTitle(string title);

		virtual void* getNativeWindowHandle() override;

		//OpenGL functions
		virtual void* GL_CreateContext();
		virtual void GL_SetVsync(Vsync setting);
		virtual Vsync GL_GetVsync();
		virtual void GL_UpdateBuffer();
		virtual void GL_DeleteContext(void* context);

	private:
		SDL_Window* window;
	};
}