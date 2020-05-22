#include "Genesis/Physics/RigidBody.hpp"

using namespace Genesis;

Rigidbody::Rigidbody()
{

}

Rigidbody::~Rigidbody()
{

}

reactphysics3d::ProxyShape* Rigidbody::addCollisionShape(reactphysics3d::CollisionShape* collision_shape, const TransformD& transform)
{
	return this->rigidbody->addCollisionShape(collision_shape, reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())), 0.0001);
}

void Rigidbody::removeCollisionShape(reactphysics3d::ProxyShape* proxy_shape)
{
	this->rigidbody->removeCollisionShape(proxy_shape);
}

void Rigidbody::setTransform(const TransformD& transform)
{
	this->rigidbody->setTransform(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())));
}

TransformD Rigidbody::getTransform()
{
	const reactphysics3d::Transform& transform_r = this->rigidbody->getTransform();
	return TransformD(toVec3D(transform_r.getPosition()), toQuatD(transform_r.getOrientation()));
}

void Rigidbody::setLinearVelocity(const vector3D& velocity)
{
	this->rigidbody->setLinearVelocity(toVec3R(velocity));
}

vector3D Rigidbody::getLinearVelocity()
{
	return toVec3D(this->rigidbody->getLinearVelocity());
}

void Rigidbody::setAngularVelocity(const vector3D& velocity)
{
	this->rigidbody->setAngularVelocity(toVec3R(velocity));
}

vector3D Rigidbody::getAngularVelocity()
{
	return toVec3D(this->rigidbody->getAngularVelocity());
}

void Rigidbody::setGravityEnabled(bool enabled)
{
	this->rigidbody->enableGravity(enabled);
}

bool Rigidbody::getGravityEnabled()
{
	return this->rigidbody->isGravityEnabled();
}

void Rigidbody::setAwake(bool awake)
{
	this->rigidbody->setIsActive(awake);
}

bool Rigidbody::getAwake()
{
	return this->rigidbody->isActive();
}

void Rigidbody::applyCenteredForce(const vector3D& force)
{
	this->rigidbody->applyForceToCenterOfMass(toVec3R(force));
}

void Rigidbody::applyForce(const vector3D& world_position, const vector3D& force)
{
	this->rigidbody->applyForce(toVec3R(force), toVec3R(world_position));
}

void Rigidbody::applyTorque(const vector3D& torque)
{
	this->rigidbody->applyTorque(toVec3R(torque));
}
