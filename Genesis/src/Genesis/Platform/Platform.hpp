#pragma once

#include "Genesis/Core/DLL.hpp"
#include "Genesis/Core/Types.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	//Prototype
	class Application;

	//Abstract platform class
	class GENESIS_DLL Platform
	{
	public:
		Platform(Application* app): application(app) {};

		virtual void onUpdate(double delta_time) = 0;

	protected:
		Application* const application;
	};
};