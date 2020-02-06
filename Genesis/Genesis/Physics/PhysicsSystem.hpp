#pragma once

#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Entity/Entity.hpp"
#include "Genesis/Job/JobSystem.hpp"

namespace Genesis
{
	namespace Physics
	{
		struct PhysicsSystem
		{
			static void prePhysicsUpdate(EntityRegistry* registry, JobSystem* job_system);
			static void postPhysicsUpdate(EntityRegistry* registry, JobSystem* job_system);
		};
	}
}