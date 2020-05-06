#include "Genesis/Physics/RigidBody.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

RigidBody::RigidBody(reactphysics3d::RigidBody* rigid_body)
{
	this->rigid_body = rigid_body;
	this->rigid_body->setType(reactphysics3d::BodyType::DYNAMIC);
}

reactphysics3d::ProxyShape* RigidBody::addCollisionShape(reactphysics3d::CollisionShape* shape, const TransformD& transform, double mass)
{
	return this->rigid_body->addCollisionShape(shape, reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())), (reactphysics3d::decimal)mass);
}

void RigidBody::removeCollisionShape(reactphysics3d::ProxyShape* shape_proxy)
{
	this->rigid_body->removeCollisionShape(shape_proxy);
}

void RigidBody::setTransform(const TransformD& transform)
{
	this->rigid_body->setTransform(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())));
}

TransformD RigidBody::getTransform()
{
	const reactphysics3d::Transform& transform_r = this->rigid_body->getTransform();
	return TransformD(toVec3D(transform_r.getPosition()), toQuatD(transform_r.getOrientation()));
}

void RigidBody::setLinearVelocity(const vector3D& velocity)
{
	this->rigid_body->setLinearVelocity(toVec3R(velocity));
}

vector3D RigidBody::getLinearVelocity()
{
	return toVec3D(this->rigid_body->getLinearVelocity());
}

void RigidBody::setAngularVelocity(const vector3D& velocity)
{
	this->rigid_body->setAngularVelocity(toVec3R(velocity));
}

vector3D RigidBody::getAngularVelocity()
{
	return toVec3D(this->rigid_body->getAngularVelocity());
}

void RigidBody::setGravityEnabled(bool enabled)
{
	this->rigid_body->enableGravity(enabled);
}

bool RigidBody::getGravityEnabled()
{
	return this->rigid_body->isGravityEnabled();
}

void RigidBody::applyCenteredForce(const vector3D& force)
{
	this->rigid_body->applyForceToCenterOfMass(toVec3R(force));
}

void RigidBody::applyForce(const vector3D& world_position, const vector3D& force)
{
	this->rigid_body->applyForce(toVec3R(force), toVec3R(world_position));
}

void RigidBody::applyTorque(const vector3D& torque)
{
	this->rigid_body->applyTorque(toVec3R(torque));
}



StaticRigidBody::StaticRigidBody(reactphysics3d::RigidBody* rigid_body)
{
	this->rigid_body = rigid_body;
	this->rigid_body->setType(reactphysics3d::BodyType::STATIC);
}

reactphysics3d::ProxyShape* StaticRigidBody::addCollisionShape(reactphysics3d::CollisionShape* shape, const TransformD& transform, double mass)
{
	return this->rigid_body->addCollisionShape(shape, reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())), (reactphysics3d::decimal)mass);
}

void StaticRigidBody::removeCollisionShape(reactphysics3d::ProxyShape* shape_proxy)
{
	this->rigid_body->removeCollisionShape(shape_proxy);
}

void StaticRigidBody::setTransform(const TransformD& transform)
{
	this->rigid_body->setTransform(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())));
}

TransformD StaticRigidBody::getTransform()
{
	const reactphysics3d::Transform& transform_r = this->rigid_body->getTransform();
	return TransformD(toVec3D(transform_r.getPosition()), toQuatD(transform_r.getOrientation()));
}



ProxyShape::ProxyShape(reactphysics3d::ProxyShape* shape_proxy)
{
	this->shape_proxy = shape_proxy;
}

void ProxyShape::setTransform(const TransformD& transform)
{
	this->shape_proxy->setLocalToBodyTransform(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())));
}

TransformD ProxyShape::getTransform()
{
	const reactphysics3d::Transform& transform_r = this->shape_proxy->getLocalToWorldTransform();
	return TransformD(toVec3D(transform_r.getPosition()), toQuatD(transform_r.getOrientation()));
}

void ProxyShape::setMass(double new_mass)
{
}

double ProxyShape::getMass()
{
	return (double)this->shape_proxy->getMass();
}