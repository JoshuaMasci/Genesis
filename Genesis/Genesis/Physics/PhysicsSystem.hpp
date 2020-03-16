#pragma once

#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Ecs/EscWorld.hpp"
#include "Genesis/Job/JobSystem.hpp"

namespace Genesis
{
	namespace Physics
	{
		struct PhysicsSystem
		{
			static void prePhysicsUpdate(EcsWorld& world, JobSystem* job_system);
			static void postPhysicsUpdate(EcsWorld& world, JobSystem* job_system);
		};
	}
}