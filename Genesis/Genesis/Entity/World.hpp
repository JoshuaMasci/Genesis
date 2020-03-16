#pragma once

#include "Genesis/Core/Application.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Scene/MeshPool.hpp"
#include "Genesis/Ecs/EscWorld.hpp"

namespace Genesis
{
	class World
	{
	public:
		World(MeshPool* mesh_pool);
		~World();

		void runSimulation(Application* application, TimeStep time_step);

		EcsWorld* world;
		EntityHandle camera;

	private:
		MeshPool* mesh_pool;
		Physics::PhysicsWorld* physics_world = nullptr;
	};
}