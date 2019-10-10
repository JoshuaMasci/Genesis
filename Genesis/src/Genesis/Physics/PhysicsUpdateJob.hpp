#pragma once

#include "Genesis/Job/Job.hpp"

namespace Genesis
{
	//TEMP
	class PhysicsWorld {};

	class PhysicsUpdateJob : public Job
	{
	public:
		PhysicsUpdateJob(PhysicsWorld* world, double delta);
		~PhysicsUpdateJob();

		virtual void run(uint32_t thread_id) override;

	protected:
		PhysicsWorld* physics_world = nullptr;
		double delta_time = 0.0;
	};
}