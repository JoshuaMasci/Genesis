#pragma once

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/Entity.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	class EntityWorld
	{
	protected:
		EntityRegistry registry;
		reactphysics3d::DynamicsWorld* physics = nullptr;

	public:
		EntityRegistry* getRegistry() { return &this->registry; };

		EntityWorld();
		~EntityWorld();

		void runSimulation(TimeStep time_step);

		Entity createEntity(const string& name);


		void onCreate();
		void onDestroy();

		void clone();
	};
}