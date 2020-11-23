#pragma once

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/EntitySystem.hpp"

namespace Genesis
{
	class EntitySystemSet
	{
	public:
		void add_system(EntitySystem* system) { this->systems.push_back(system); };
		void run_systems(EntityWorld* world, const TimeStep time_step)
		{
			for (auto system : this->systems)
			{
				system->run(world, time_step);
			}
		};

	protected:
		vector<EntitySystem*> systems;
	};
}