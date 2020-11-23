#pragma once

#include "Genesis/Ecs/EntityWorld.hpp"

namespace Genesis
{
	class EntitySystem
	{
	public:
		virtual void run(EntityWorld* world, const TimeStep time_step) = 0;
	};
}