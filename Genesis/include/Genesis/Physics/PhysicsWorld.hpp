#pragma once

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	class Entity;

	class PhysicsWorld
	{
	public:
		PhysicsWorld(vector3D gravity = vector3D(0.0));
		virtual ~PhysicsWorld();

		void simulate(TimeStep time_step);

		void addEntity(Entity* entity);
		void removeEntity(Entity* entity);

	private:
		reactphysics3d::DynamicsWorld* dynamics_world = nullptr;
	};
}