#include "RigidBody.hpp"

#include "Genesis/World/Entity.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"

using namespace Genesis;

RigidBody::RigidBody(Entity* entity)
{
	this->parent = entity;
}

RigidBody::~RigidBody()
{
	if (this->rigidBody != nullptr)
	{
		delete this->rigidBody;
	}
}

void RigidBody::setMass(double mass)
{
	this->rigidBody->setMassProps(mass, this->rigidBody->getLocalInertia());
	this->mass = mass;
}

double RigidBody::getMass()
{
	return this->mass;
}

void RigidBody::setInertiaTensor(vector3D inertia)
{
	this->rigidBody->setMassProps(getMass(), toBtVec3(inertia));

}

vector3D RigidBody::getInertiaTensor()
{
	return toVec3(this->rigidBody->getLocalInertia());
}

void RigidBody::Activate(bool activate)
{
	this->rigidBody->activate(activate);
}

Transform RigidBody::getWorldTransform()
{
	return toTransform(this->rigidBody->getWorldTransform());
}

void RigidBody::setWorldTransform(Transform transform)
{
	this->rigidBody->setCenterOfMassTransform(toBtTransform(transform));
}

vector3D RigidBody::getLinearVelocity() const
{
	return toVec3(this->rigidBody->getLinearVelocity());
}

void RigidBody::setLinearVelocity(vector3D velocity)
{
	this->rigidBody->setLinearVelocity(toBtVec3(velocity));
}

vector3D RigidBody::getAngularVelocity() const
{
	return toVec3(this->rigidBody->getAngularVelocity());
}

void RigidBody::setAngularVelocity(vector3D velocity)
{
	this->rigidBody->setAngularVelocity(toBtVec3(velocity));
}

void RigidBody::applyForce(vector3D &force, vector3D &localPos)
{
	this->rigidBody->activate(true);
	this->rigidBody->applyForce(toBtVec3(force), toBtVec3(localPos));
}

void RigidBody::applyImpulse(vector3D &impulse, vector3D &localPos)
{
	this->rigidBody->activate(true);
	this->rigidBody->applyImpulse(toBtVec3(impulse), toBtVec3(localPos));
}

void RigidBody::applyCentralForce(vector3D &force)
{
	this->rigidBody->activate(true);
	this->rigidBody->applyCentralForce(toBtVec3(force));
}

void RigidBody::applyCentralImpulse(vector3D &impulse)
{
	this->rigidBody->activate(true);
	this->rigidBody->applyCentralImpulse(toBtVec3(impulse));
}

void RigidBody::applyTorque(vector3D &torque)
{
	this->rigidBody->activate(true);
	this->rigidBody->applyTorque(toBtVec3(torque));
}

void RigidBody::applyTorqueImpulse(vector3D &torque)
{
	this->rigidBody->activate(true);
	this->rigidBody->applyTorqueImpulse(toBtVec3(torque));
}

void RigidBody::setDampening(double linear, double angular)
{
	rigidBody->setDamping(linear, angular);
}

bool RigidBody::isInWorld()
{
	if (this->physics_world != nullptr)
	{
		return true;
	}

	return false;
}

void RigidBody::setCollisionShape(btCollisionShape* shape)
{
	if (this->physics_world != nullptr)
	{
		PhysicsWorld* world = this->physics_world;
		this->physics_world->removeRigidBody(this);
		this->rigidBody->setCollisionShape(shape);
		world->addRigidBody(this);
	}
	else
	{
		this->rigidBody->setCollisionShape(shape);
	}
}
