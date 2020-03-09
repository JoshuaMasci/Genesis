#pragma once

#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Core/Application.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Scene/MeshPool.hpp"

namespace Genesis
{
	class World
	{
	public:
		World(MeshPool* mesh_pool);
		~World();

		void runSimulation(Application* application, TimeStep time_step);

		inline EntityRegistry& getRegistry() { return this->entity_registry; };
		inline EntityId getCameraEntity() { return this->camera; };

	private:
		MeshPool* mesh_pool;

		EntityRegistry entity_registry;
		EntityId camera;

		Physics::PhysicsWorld* physics_world = nullptr;
	};
}