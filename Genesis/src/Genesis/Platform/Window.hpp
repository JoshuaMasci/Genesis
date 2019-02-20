#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	enum class VSYNC
	{
		ADAPTIVE = -1,
		OFF = 0,
		ON = 1
	};

	//Abstract window class
	class Window
	{
	public:
		Window(vector2I size, string title) {};

		virtual vector2I getWindowSize() = 0;
		virtual void setWindowSize(vector2I size) = 0;

		virtual void setWindowTitle(string title) = 0;

		virtual void updateBuffer() = 0;
	};
};