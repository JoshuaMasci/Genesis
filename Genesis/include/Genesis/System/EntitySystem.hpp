#pragma once

#include "Genesis/Scene/Scene.hpp"

namespace Genesis
{
	class EntitySystem
	{
	public:
		virtual void run(Scene* scene, const TimeStep time_step) = 0;
	};
}