#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	//Abstract window class
	class Window
	{
	public:
		Window(vector2I size, string title) {};
		virtual ~Window() {};

		virtual vector2I getWindowSize() = 0;
		virtual void setWindowSize(vector2I size) = 0;

		virtual void setWindowTitle(string title) = 0;
	};
};