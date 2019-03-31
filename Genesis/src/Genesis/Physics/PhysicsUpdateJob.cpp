#include "PhysicsUpdateJob.hpp"

using namespace Genesis;

Genesis::PhysicsUpdateJob::PhysicsUpdateJob(PhysicsWorld* world, double delta)
{
	this->physics_world = world;
	this->delta_time = delta;
}

Genesis::PhysicsUpdateJob::~PhysicsUpdateJob()
{
	//Nothing
}

void Genesis::PhysicsUpdateJob::run()
{
	if (this->physics_world != nullptr)
	{
		this->physics_world->runSimulation(this->delta_time);
	}
}
