#pragma once

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	struct RaycastResult
	{
		bool has_hit = false;
		vector3D hit_point;
		vector3D hit_normal;
	};

	class PhysicsWorld
	{
	public:
		PhysicsWorld(vector3D gravity = vector3D(0.0));
		virtual ~PhysicsWorld();

		void simulate(TimeStep time_step);

		void castRay(vector3D start_position, vector3D end_pos);

	private:
		reactphysics3d::DynamicsWorld* dynamics_world = nullptr;
	};
}