#pragma once

#include "genesis_engine/platform/Window.hpp"

namespace genesis_engine
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