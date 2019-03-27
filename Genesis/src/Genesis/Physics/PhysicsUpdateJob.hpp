#pragma once

#include "Genesis/Job/Job.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"

namespace Genesis
{
	class PhysicsUpdateJob : public Job
	{
	public:
		PhysicsUpdateJob(PhysicsWorld* world, double delta);
		~PhysicsUpdateJob();

		virtual void run();

	protected:
		PhysicsWorld* physics_world = nullptr;
		double delta_time = 0.0;
	};
}