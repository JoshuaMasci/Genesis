#include "Genesis/Physics/PhysicsWorld.hpp"

#include "Genesis/Entity/Entity.hpp"
#include "Genesis/Physics/Rigidbody.hpp"

using namespace Genesis;

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

void PhysicsWorld::addEntity(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "Physics World tried to add a null entity");
	GENESIS_ENGINE_ASSERT_ERROR(entity->isRoot(), "Entity must be root to have a rigidbody");
	GENESIS_ENGINE_ASSERT_ERROR(entity->getRigidbody() != nullptr, "Physics World tried to add a null rigidbody");
	
	TransformD& transform = entity->getLocalTransform();
	reactphysics3d::Transform react_transform;
	react_transform.setPosition(toVec3R(transform.getPosition()));
	react_transform.setOrientation(toQuatR(transform.getOrientation()));

	Rigidbody* rigidbody = entity->getRigidbody();
	rigidbody->setHandle(this->dynamics_world->createRigidBody(react_transform));
}

void PhysicsWorld::removeEntity(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "Physics World tried to remove a null entity");
	GENESIS_ENGINE_ASSERT_ERROR(entity->getRigidbody() != nullptr, "Physics World tried to remove a null rigidbody");

	Rigidbody* rigidbody = entity->getRigidbody();
	reactphysics3d::RigidBody* react_rigidbody = rigidbody->getHandle();
	this->dynamics_world->destroyRigidBody(react_rigidbody);
	rigidbody->setHandle(nullptr);
}