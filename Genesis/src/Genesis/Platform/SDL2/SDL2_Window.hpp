#pragma once

#include "Genesis/Platform/Window.hpp"

//Prototype struct
struct SDL_Window;

namespace Genesis
{
	class SDL2_Window: public Window
	{
	public:
		SDL2_Window(vector2U size, string title);
		virtual ~SDL2_Window();

		virtual vector2U getWindowSize();
		virtual void setWindowSize(vector2U size);

		virtual void setWindowTitle(string title);

		virtual void updateBuffer();

	private:
		SDL_Window* window;

		// Inherited via Window
		virtual void * getNativeWindowHandle() override;
	};
}