#pragma once

namespace reactphysics3d
{
	class RigidBody;
	class ProxyShape;
	class CollisionShape;
}

namespace Genesis
{
	enum class RigidBodyType
	{
		Static = 0,
		Kinematic = 1,
		Dynamic = 2,
	};

	class RigidBody
	{
	public:
		void attachRigidBody(reactphysics3d::RigidBody* rigidbody);
		reactphysics3d::RigidBody* removeRigidBody();
		bool hasRigidBody();

		void setType(RigidBodyType type);
		RigidBodyType getType();

		reactphysics3d::ProxyShape* addShape(reactphysics3d::CollisionShape* shape, const TransformD& transform, double mass);

		void setTransform(const TransformD& transform);
		void getTransform(TransformD& transform);

		void setMass(double mass);
		double getMass();

		void setGravityEnabled(bool enabled);
		bool getGravityEnabled();

		void setIsAllowedToSleep(bool awake);
		bool getIsAllowedToSleep();

		void setLinearVelocity(const vector3D& velocity);
		vector3D getLinearVelocity();

		void setAngularVelocity(const vector3D& velocity);
		vector3D getAngularVelocity();

		void applyCenteredForce(const vector3D& force);
		void applyForce(const vector3D& world_position, const vector3D& force);
		void applyTorque(const vector3D& torque);

	protected:
		reactphysics3d::RigidBody* rigidbody = nullptr;

		RigidBodyType type = RigidBodyType::Dynamic;
		double body_mass = 1.0f;
		bool gravity_enabled = true;
		bool is_allowed_to_sleep = true;
		vector3D linear_velocity = vector3D(0.0);
		vector3D angular_velocity = vector3D(0.0);
	};
}