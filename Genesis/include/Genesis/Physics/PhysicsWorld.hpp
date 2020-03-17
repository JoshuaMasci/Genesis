#pragma once

#include "Genesis/Core/Types.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	namespace Physics
	{
		class PhysicsWorld
		{
		public:
			PhysicsWorld(vector3D gravity = vector3D(0.0));
			virtual ~PhysicsWorld();

			void simulate(TimeStep time_step);

			reactphysics3d::RigidBody* addRigidBody(const TransformD& transform);
			void removeRigidBody(reactphysics3d::RigidBody* rigid_body);

		private:
			reactphysics3d::DynamicsWorld* dynamics_world = nullptr;
		};
	}
}