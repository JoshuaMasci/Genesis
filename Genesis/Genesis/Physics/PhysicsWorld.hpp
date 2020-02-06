#pragma once

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	namespace Physics
	{
		struct PhysicsWorld
		{
			reactphysics3d::DynamicsWorld* world;
		};
	}
}