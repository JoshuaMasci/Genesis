#pragma once

#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Core/Types.hpp"
#include "Genesis/WorldTransform.hpp"
#include "Genesis/Entity.hpp"
#include "Genesis/Job/JobSystem.hpp"

namespace Genesis
{
	class PhysicsSystem
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void runSimulation(EntityRegistry& registry, JobSystem& job_system, double delta_time);

	private:
		void PrePhysicsUpdate(EntityRegistry& registry, JobSystem& job_system, double delta_time);
		void PhysicsUpdate(EntityRegistry& registry, JobSystem& job_system, double delta_time);
		void PostPhysicsUpdate(EntityRegistry& registry, JobSystem& job_system, double delta_time);

		PhysicsWorld* physics_world;
	};
}