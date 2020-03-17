#include "Genesis/Physics/PhysicsWorld.hpp"

#include "Genesis/Debug/Profiler.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

PhysicsWorld::PhysicsWorld(vector3D gravity)
{
	this->dynamics_world = new reactphysics3d::DynamicsWorld(toVec3R(gravity));
}

PhysicsWorld::~PhysicsWorld()
{
	delete this->dynamics_world;
}

void PhysicsWorld::simulate(TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("PhysicsWorld::simulate");
	this->dynamics_world->update((reactphysics3d::decimal)time_step);
}

reactphysics3d::RigidBody* PhysicsWorld::addRigidBody(const TransformD& transform)
{
	reactphysics3d::Transform react_transform;
	react_transform.setPosition(toVec3R(transform.getPosition()));
	react_transform.setOrientation(toQuatR(transform.getOrientation()));
	return this->dynamics_world->createRigidBody(react_transform);
}

void PhysicsWorld::removeRigidBody(reactphysics3d::RigidBody* rigid_body)
{
	this->dynamics_world->destroyRigidBody(rigid_body);
}
