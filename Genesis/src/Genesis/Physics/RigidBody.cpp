#include "RigidBody.hpp"

#include "Genesis/Physics/PhysicsWorld.hpp"

using namespace Genesis;

RigidBody::RigidBody()
{
	this->empty_shape = new btEmptyShape();
	
	btDefaultMotionState* motion_state = new btDefaultMotionState();
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(1.0, motion_state, this->empty_shape, btVector3(1.0, 1.0, 1.0));
	this->rigid_body = new btRigidBody(boxRigidBodyCI);
}

RigidBody::~RigidBody()
{
	if (this->physics_world != nullptr)
	{
		this->physics_world->removeRigidBody(this);
	}

	if (this->rigid_body != nullptr)
	{
		delete this->rigid_body;
	}

	delete this->empty_shape;
}

Transform RigidBody::getWorldTransform()
{
	return toTransform(this->rigid_body->getWorldTransform());
}

void RigidBody::setWorldTransform(const Transform& transform)
{
	this->rigid_body->setCenterOfMassTransform(toBtTransform(transform));
}

void RigidBody::setCollisionShape(btCollisionShape* shape)
{
	PhysicsWorld* world = this->physics_world;

	if (world != nullptr)
	{
		this->physics_world->removeRigidBody(this);
	}

	if (shape != nullptr)
	{
		this->rigid_body->setCollisionShape(shape);
	}
	else
	{
		this->rigid_body->setCollisionShape(this->empty_shape);
	}

	if (world != nullptr)
	{
		world->addRigidBody(this);
	}
}

void RigidBody::addToWorld(PhysicsWorld* world)
{
	if (this->physics_world != nullptr)
	{
		//TODO better errors
		printf("Error: already in a physics world\n");
		exit(1);
	}

	world->addRigidBody(this);
	this->physics_world = world;
}

void RigidBody::removeFromWorld()
{
	if (this->physics_world != nullptr)
	{
		this->physics_world->removeRigidBody(this);
		this->physics_world = nullptr;
	}
	else
	{
		//TODO better errors
		printf("Error: not in a physics world\n");
		exit(1);
	}
}

