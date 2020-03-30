#pragma once

#include "Genesis/Core/Application.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"
#include "Genesis/Entity/EntitySystem.hpp"

namespace Genesis
{
	class World
	{
	public:
		World(MeshPool* mesh_pool, MaterialPool* material_pool);
		~World();

		void runSimulation(Application* application, TimeStep time_step);

		inline EntityRegistry* getEntityRegistry() { return this->entity_registry; };
		inline EntityHandle getCamera() { return this->main_camera; };

	private:
		MeshPool* mesh_pool;
		MaterialPool* material_pool;

		EntityRegistry* entity_registry = nullptr;
		Physics::PhysicsWorld* physics_world = nullptr;


		EntityHandle main_camera;
	};
}