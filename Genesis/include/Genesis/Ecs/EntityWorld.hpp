#pragma once

#include "Genesis/Ecs/Ecs.hpp"
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

		void resolveTransforms();
		void runSimulation(TimeStep time_step);

		void onCreate();
		void onDestroy();

		void clone();
	};
}