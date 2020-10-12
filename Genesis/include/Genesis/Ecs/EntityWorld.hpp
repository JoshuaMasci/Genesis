#pragma once

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/Entity.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	class EntityWorld
	{
	public:
		EntityRegistry* getRegistry() { return &this->registry; };

		EntityWorld();
		~EntityWorld();

		void runSimulation(TimeStep time_step);

		void updateTransforms();

		Entity createEntity();
		Entity createEntity(const string& name);

		void destroyEntity(Entity entity);

		void onCreate();
		void onDestroy();

	protected:
		EntityRegistry registry;
		reactphysics3d::DynamicsWorld* physics = nullptr;

		void updateEntityTreeRoot(EntityHandle entity);
		void updateEntityTree(EntityHandle entity, const TransformD& parent_world_transform, const TransformD& parent_relative_transform);
	};
}