#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	enum class GraphicsAPI
	{
		OpenGL,
		Vulkan,
		DirectX11,
		DirectX12,
		Metal,
	};

	enum class Vsync
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
		virtual ~Window() {};

		virtual vector2U getWindowSize() = 0;
		virtual void setWindowSize(vector2U size) = 0;

		virtual void setWindowTitle(string title) = 0;

		virtual void* getNativeWindowHandle() = 0;

		//OpenGL functions
		virtual void* GL_CreateContext() = 0;
		virtual void GL_SetVsync(Vsync setting) = 0;
		virtual Vsync GL_GetVsync() = 0;
		virtual void GL_UpdateBuffer() = 0;
		virtual void GL_DeleteContext(void* context) = 0;
	};
};