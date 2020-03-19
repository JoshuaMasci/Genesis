#pragma once

#include "Genesis/Core/Application.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/EntitySystem/EntityRegistry.hpp"

namespace Genesis
{
	class World
	{
	public:
		World(MeshPool* mesh_pool);
		~World();

		void runSimulation(Application* application, TimeStep time_step);

		EntitySystem::EntityRegistry* world;
		EntityHandle camera;

	private:
		MeshPool* mesh_pool;
		Physics::PhysicsWorld* physics_world = nullptr;
	};
}