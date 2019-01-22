#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	//Prototype
	class Application;

	class Layer
	{
	public:
		Layer(string name) :layer_name(name) {};

		virtual void attach(Application* app) = 0;
		virtual void detach(Application* app) = 0;
		virtual void onUpdate(double delta_time, Application* app) = 0;

	protected:
		string layer_name;
	};
};