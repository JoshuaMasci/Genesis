#include "PhysicsUpdateJob.hpp"

using namespace Genesis;

PhysicsUpdateJob::PhysicsUpdateJob(PhysicsWorld* world, double delta)
{
	this->physics_world = world;
	this->delta_time = delta;
}

PhysicsUpdateJob::~PhysicsUpdateJob()
{
	//Nothing
}

void PhysicsUpdateJob::run(uint32_t thread_id)
{
	if (this->physics_world != nullptr)
	{
		//this->physics_world->runSimulation(this->delta_time);
	}
}
