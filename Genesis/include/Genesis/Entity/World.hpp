#pragma once

#include "Genesis/Core/Application.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Entity/EntitySystem.hpp"
#include "Genesis/Rendering/BaseWorldRenderer.hpp"

namespace Genesis
{
	class World
	{
	public:
		World(BaseWorldRenderer* world_renderer);
		~World();

		void runSimulation(Application* application, TimeStep time_step);

		inline EntityRegistry* getEntityRegistry() { return this->entity_registry; };
		inline EntityHandle getCamera() { return this->main_camera; };

	private:
		BaseWorldRenderer* world_renderer;

		EntityRegistry* entity_registry = nullptr;
		Physics::PhysicsWorld* physics_world = nullptr;


		EntityHandle main_camera;
	};
}