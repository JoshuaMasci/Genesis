#pragma once

#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	//Prototype
	class Application;

	//Abstract platform class
	class Platform
	{
	public:
		Platform(Application* app): application(app) {};

		virtual void onUpdate(TimeStep time_step) = 0;

	protected:
		Application* const application;
	};
};