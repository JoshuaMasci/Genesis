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

	private:
		MeshPool* mesh_pool;

		Physics::PhysicsWorld* physics_world = nullptr;
	};
}