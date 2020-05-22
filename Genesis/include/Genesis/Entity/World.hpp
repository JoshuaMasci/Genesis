#pragma once

#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Entity/EntitySystem.hpp"
#include "Genesis/Rendering/BaseWorldRenderer.hpp"

namespace Genesis
{
	class Entity;

	class World
	{
	public:
		World(BaseWorldRenderer* world_renderer);
		~World();

		void runSimulation(Application* application, TimeStep time_step);

		inline EntityRegistry* getEntityRegistry() { return this->entity_registry; };
		inline EntityHandle getCamera() { return this->main_camera; };

		void addEntity(Entity* entity);
		void removeEntity(Entity* entity);
		inline const vector<Entity*>& getEntities() { return this->root_entities; };

		inline PhysicsWorld* getPhysicsWorld() { return this->physics_world; };

	private:
		BaseWorldRenderer* world_renderer;

		EntityRegistry* entity_registry = nullptr;
		PhysicsWorld* physics_world = nullptr;

		EntityHandle main_camera;

		vector<Entity*> root_entities;
	};
}