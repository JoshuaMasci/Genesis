#pragma once

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/Entity.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	class EntityWorld
	{
	public:
		EntityWorld();
		~EntityWorld();

		void runSimulation(TimeStep time_step);
		void resolveTransforms();

		Entity createEntity();
		Entity createEntity(const string& name);

		void destroyEntity(Entity entity);

		Entity getEntity(EntityHandle entity_handle);

		void onCreate();
		void onDestroy();

		EntityRegistry* getRegistry() { return &this->registry; };
		reactphysics3d::DynamicsWorld* getPhysicsWorld() { return this->physics; };

	protected:
		EntityRegistry registry;
		reactphysics3d::DynamicsWorld* physics = nullptr;
	};
}