#pragma once

#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/EntitySystem/EntityRegistry.hpp"
#include "Genesis/Job/JobSystem.hpp"

namespace Genesis
{
	namespace Physics
	{
		struct PhysicsSystem
		{
			static void prePhysicsUpdate(EntitySystem::EntityRegistry& world, JobSystem* job_system);
			static void postPhysicsUpdate(EntitySystem::EntityRegistry& world, JobSystem* job_system);
		};
	}
}