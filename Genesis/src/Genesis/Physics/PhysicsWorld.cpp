#include "PhysicsWorld.hpp"

using namespace Genesis;

PhysicsWorld::PhysicsWorld()
{
	this->dynamics_world = new reactphysics3d::DynamicsWorld(reactphysics3d::Vector3(0.0, 0.0, 0.0));
}

PhysicsWorld::~PhysicsWorld()
{
	delete this->dynamics_world;
}

void PhysicsWorld::update(double delta_time)
{
	this->dynamics_world->update(delta_time);
}