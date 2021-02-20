#pragma once

#include "Genesis/Scene/Ecs.hpp"
#include "Genesis/System/EntitySystem.hpp"

namespace Genesis
{
	class EntitySystemSet
	{
	public:
		void add_system(EntitySystem* system) { this->systems.push_back(system); };
		void run_systems(Scene* scene, const TimeStep time_step)
		{
			for (auto system : this->systems)
			{
				system->run(scene, time_step);
			}
		};

	protected:
		vector<EntitySystem*> systems;
	};
}