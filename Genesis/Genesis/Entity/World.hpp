#pragma once

#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Core/Application.hpp"

namespace Genesis
{
	class World
	{
	public:
		World();
		~World();

		void runSimulation(Application* application, double delta_time);

		EntityRegistry entity_registry;
		EntityId camera;
	};
}