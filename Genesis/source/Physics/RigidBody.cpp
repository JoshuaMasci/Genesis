#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	void RigidBody::attachRigidBody(reactphysics3d::RigidBody* rigidbody)
	{
		this->rigidbody = rigidbody;
		this->rigidbody->setMass(this->mass);
		this->rigidbody->setLinearVelocity(toVec3R(this->linear_velocity));
		this->rigidbody->setAngularVelocity(toVec3R(this->angular_velocity));
		this->rigidbody->enableGravity(this->gravity_enabled);
		this->rigidbody->setIsActive(this->awake);
	}
	
	reactphysics3d::RigidBody* RigidBody::removeRigidBody()
	{
		this->mass = this->rigidbody->getMass();
		this->linear_velocity = toVec3D(this->rigidbody->getLinearVelocity());
		this->angular_velocity = toVec3D(this->rigidbody->getAngularVelocity());
		this->gravity_enabled = this->rigidbody->isGravityEnabled();
		this->awake = this->rigidbody->isActive();
		reactphysics3d::RigidBody* temp = this->rigidbody;
		this->rigidbody = nullptr;
		return temp;
	}

	bool RigidBody::hasRigidBody()
	{
		return this->rigidbody != nullptr;
	}

	void RigidBody::setTransform(const TransformD& transform)
	{
		this->rigidbody->setTransform(reactphysics3d::Transform(toVec3R(transform.getPosition()), toQuatR(transform.getOrientation())));
	}

	TransformD RigidBody::getTransform()
	{
		const reactphysics3d::Transform& transform_r = this->rigidbody->getTransform();
		return TransformD(toVec3D(transform_r.getPosition()), toQuatD(transform_r.getOrientation()));
	}

	void RigidBody::setMass(double mass)
	{
		this->mass = mass;

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->setMass(this->mass);
		}
	}

	double RigidBody::getMass()
	{
		if (this->rigidbody != nullptr)
		{
			this->mass = this->rigidbody->getMass();
		}

		return this->mass;
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

	void RigidBody::setAwake(bool awake)
	{
		this->awake = awake;

		if (this->rigidbody != nullptr)
		{
			this->rigidbody->setIsActive(this->awake);
		}
	}

	bool RigidBody::getAwake()
	{
		if (this->rigidbody != nullptr)
		{
			this->awake = this->rigidbody->isActive();
		}

		return this->awake;
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