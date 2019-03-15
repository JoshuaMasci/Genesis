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
		Window(vector2U size, string title) {};

		virtual vector2U getWindowSize() = 0;
		virtual void setWindowSize(vector2U size) = 0;

		virtual void setWindowTitle(string title) = 0;

		virtual void updateBuffer() = 0;

		virtual void* getNativeWindowHandle() = 0;
	};
};