#pragma once

#include "Genesis/Platform/Window.hpp"
#include "Genesis/Platform/SDL2/SDL2_Include.hpp"

namespace Genesis
{
	class SDL2_Window: public Window
	{
	public:
		SDL2_Window(vector2I size, string title);
		virtual ~SDL2_Window();

		virtual vector2I getWindowSize();
		virtual void setWindowSize(vector2I size);

		virtual void setWindowTitle(string title);
	private:
		SDL_Window* window;
	};
}