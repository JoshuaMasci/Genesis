#pragma once

#include "Genesis/Platform/Window.hpp"

//Prototype struct
struct SDL_Window;

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

		//Going to move later
		virtual void setVsync(VSYNC setting) override;
		virtual VSYNC getVsync() override;

	private:
		SDL_Window* window;
	};
}