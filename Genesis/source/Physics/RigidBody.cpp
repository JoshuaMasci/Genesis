#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	void RigidBody::setType(RigidBodyType type)
	{
		this->rigidbody->setType((reactphysics3d::BodyType) type);
	}

	RigidBodyType RigidBody::getType()
	{
		return (RigidBodyType) this->rigidbody->getType();
	}

	reactphysics3d::ProxyShape* RigidBody::addShape(reactphysics3d::CollisionShape* shape, const TransformD& transform)
	{
		return this->rigidbody->addCollisionShape(shape, reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())), 0.0);
	}

	void RigidBody::setTransform(const TransformD& transform)
	{
		this->rigidbody->setTransform(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())));
	}

	void RigidBody::getTransform(TransformD& transform)
	{
		const reactphysics3d::Transform& transform_r = this->rigidbody->getTransform();
		transform.setPosition(toVec3D(transform_r.getPosition()));
		transform.setOrientation(toQuatD(transform_r.getOrientation()));
	}

	void RigidBody::setMass(double mass)
	{
		this->rigidbody->setMass(mass);
	}

	double RigidBody::getMass()
	{
		return this->rigidbody->getMass();
	}

	void RigidBody::setGravityEnabled(bool enabled)
	{
		this->rigidbody->enableGravity(enabled);
	}

	bool RigidBody::getGravityEnabled()
	{
		return this->rigidbody->isGravityEnabled();
	}

	void RigidBody::setIsAllowedToSleep(bool awake)
	{
		this->rigidbody->setIsAllowedToSleep(awake);
	}

	bool RigidBody::getIsAllowedToSleep()
	{
		return this->rigidbody->isAllowedToSleep();
	}

	void RigidBody::setLinearVelocity(const vector3D& velocity)
	{
		this->rigidbody->setLinearVelocity(toVec3R(velocity));
	}

	vector3D RigidBody::getLinearVelocity()
	{
		return toVec3D(this->rigidbody->getLinearVelocity());
	}

	void RigidBody::setAngularVelocity(const vector3D& velocity)
	{
		this->rigidbody->setAngularVelocity(toVec3R(velocity));
	}

	vector3D RigidBody::getAngularVelocity()
	{
		return toVec3D(this->rigidbody->getAngularVelocity());
	}

	void RigidBody::applyCenteredForce(const vector3D& force)
	{
		this->rigidbody->applyForceToCenterOfMass(toVec3R(force));
	}

	void RigidBody::applyForce(const vector3D& world_position, const vector3D& force)
	{
		this->rigidbody->applyForce(toVec3R(force), toVec3R(world_position));
	}

	void RigidBody::applyTorque(const vector3D& torque)
	{
		this->rigidbody->applyTorque(toVec3R(torque));
	}
}