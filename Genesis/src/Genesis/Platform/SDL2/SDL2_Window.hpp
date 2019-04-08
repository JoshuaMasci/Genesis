#pragma once

#include "Genesis/Platform/Window.hpp"

//Prototype struct
struct SDL_Window;

namespace Genesis
{
	class SDL2_Window: public Window
	{
	public:
		SDL2_Window(vector2U size, string title, GraphicsAPI api);
		virtual ~SDL2_Window();

		virtual vector2U getWindowSize();
		virtual void setWindowSize(vector2U size);

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