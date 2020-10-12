#pragma once

namespace reactphysics3d
{
	class RigidBody;
	class ProxyShape;
	class CollisionShape;
}

namespace Genesis
{
	class RigidBody
	{
	protected:
		double mass = 1.0f;
		bool gravity_enabled = true;
		bool awake = true;
		vector3D linear_velocity = vector3D(0.0);
		vector3D angular_velocity = vector3D(0.0);

		reactphysics3d::RigidBody* rigidbody = nullptr;

	public:
		void attachRigidBody(reactphysics3d::RigidBody* rigidbody);
		reactphysics3d::RigidBody* removeRigidBody();
		bool hasRigidBody();
		//reactphysics3d::RigidBody* getRigidBody();

		reactphysics3d::ProxyShape* addShape(reactphysics3d::CollisionShape* shape, const TransformD& transform);

		void setTransform(const TransformD& transform);
		TransformD getTransform();

		void setMass(double mass);
		double getMass();

		void setGravityEnabled(bool enabled);
		bool getGravityEnabled();

		void setAwake(bool awake);
		bool getAwake();

		void setLinearVelocity(const vector3D& velocity);
		vector3D getLinearVelocity();

		void setAngularVelocity(const vector3D& velocity);
		vector3D getAngularVelocity();

		void applyCenteredForce(const vector3D& force);
		void applyForce(const vector3D& world_position, const vector3D& force);
		void applyTorque(const vector3D& torque);
	};
}