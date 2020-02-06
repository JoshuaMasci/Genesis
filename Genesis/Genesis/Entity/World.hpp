#pragma once

#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Core/Application.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"


namespace Genesis
{
	class World
	{
	public:
		World();
		~World();

		void runSimulation(Application* application, TimeStep time_step);

		EntityRegistry entity_registry;
		EntityId camera;

	private:
		Physics::PhysicsWorld* physics_world = nullptr;
	};
}