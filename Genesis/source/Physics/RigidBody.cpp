#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	void RigidBody::attachRigidBody(reactphysics3d::RigidBody* rigidbody)
	{
		this->rigidbody = rigidbody;
		this->rigidbody->setType((reactphysics3d::BodyType) this->type);
		this->rigidbody->setMass(this->body_mass);
		this->rigidbody->setLinearVelocity(toVec3R(this->linear_velocity));
		this->rigidbody->setAngularVelocity(toVec3R(this->angular_velocity));
		this->rigidbody->enableGravity(this->gravity_enabled);
		this->rigidbody->setIsAllowedToSleep(this->is_allowed_to_sleep);
	}

	reactphysics3d::RigidBody* RigidBody::removeRigidBody()
	{
		this->type = (RigidBodyType) this->rigidbody->getType();
		this->body_mass = this->rigidbody->getMass();
		this->linear_velocity = toVec3D(this->rigidbody->getLinearVelocity());
		this->angular_velocity = toVec3D(this->rigidbody->getAngularVelocity());
		this->gravity_enabled = this->rigidbody->isGravityEnabled();
		this->is_allowed_to_sleep = this->rigidbody->isAllowedToSleep();
		reactphysics3d::RigidBody* temp = this->rigidbody;
		this->rigidbody = nullptr;
		return temp;
	}

	bool RigidBody::hasRigidBody()
	{
		return this->rigidbody != nullptr;
	}

	void RigidBody::setType(RigidBodyType type)
	{
		this->type = type;

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->setType((reactphysics3d::BodyType) this->type);
		}
	}

	RigidBodyType RigidBody::getType()
	{
		if (this->rigidbody != nullptr)
		{
			this->type = (RigidBodyType)this->rigidbody->getType();
		}
		return this->type;
	}

	reactphysics3d::ProxyShape* RigidBody::addShape(reactphysics3d::CollisionShape* shape, const TransformD& transform, double mass)
	{
		return this->rigidbody->addCollisionShape(shape, reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())), mass);
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
		this->body_mass = mass;

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->setMass(this->body_mass);
		}
	}

	double RigidBody::getMass()
	{
		if (this->rigidbody != nullptr)
		{
			this->body_mass = this->rigidbody->getMass();
		}

		return this->body_mass;
	}

	void RigidBody::setGravityEnabled(bool enabled)
	{
		this->gravity_enabled = enabled;

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->enableGravity(this->gravity_enabled);
		}
	}

	bool RigidBody::getGravityEnabled()
	{
		if (this->rigidbody != nullptr)
		{
			this->gravity_enabled = this->rigidbody->isGravityEnabled();
		}

		return this->gravity_enabled;
	}

	void RigidBody::setIsAllowedToSleep(bool awake)
	{
		this->is_allowed_to_sleep = awake;

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->setIsActive(this->is_allowed_to_sleep);
		}
	}

	bool RigidBody::getIsAllowedToSleep()
	{
		if (this->rigidbody != nullptr)
		{
			this->is_allowed_to_sleep = this->rigidbody->isActive();
		}

		return this->is_allowed_to_sleep;
	}

	void RigidBody::setLinearVelocity(const vector3D& velocity)
	{
		this->linear_velocity = velocity;

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->setLinearVelocity(toVec3R(this->linear_velocity));
		}
	}

	vector3D RigidBody::getLinearVelocity()
	{
		if (this->rigidbody != nullptr)
		{
			this->linear_velocity = toVec3D(this->rigidbody->getLinearVelocity());
		}

		return this->linear_velocity;
	}

	void RigidBody::setAngularVelocity(const vector3D& velocity)
	{
		this->angular_velocity = velocity;

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->setAngularVelocity(toVec3R(this->angular_velocity));
		}
	}

	vector3D RigidBody::getAngularVelocity()
	{
		if (this->rigidbody != nullptr)
		{
			this->angular_velocity = toVec3D(this->rigidbody->getAngularVelocity());
		}

		return this->angular_velocity;
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