#pragma once

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	struct RigidBody
	{
		vector3D linear_velocity;
		vector3D angular_velocity;

		double mass;

		reactphysics3d::RigidBody* rigid_body;
	};
}