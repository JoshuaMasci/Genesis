#pragma once

#include "Genesis/Physics/ReactPhyscis.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"

namespace Genesis
{
	namespace Physics
	{
		enum class RigidBodyType
		{
			Dynamic,
			Kinematic,
			Static,
		};

		class RigidBody
		{
		public:
			RigidBody(PhysicsWorld* physics_world, const TransformD& transform);
			~RigidBody();

			void setType(RigidBodyType type);

			reactphysics3d::ProxyShape* addCollisionShape(reactphysics3d::CollisionShape* shape, const TransformD& transform, double mass);
			void removeCollisionShape(reactphysics3d::ProxyShape* shape_proxy);

			void setTransform(const TransformD& transform);
			TransformD getTransform();

			void setLinearVelocity(const vector3D& velocity);
			vector3D getLinearVelocity();

			void setAngularVelocity(const vector3D& velocity);
			vector3D getAngularVelocity();

			void setGravityEnabled(bool enabled);
			bool getGravityEnabled();

			void applyCenteredForce(const vector3D& force);
			void applyForce(const vector3D& world_position, const vector3D& force);
			void applyTorque(const vector3D& torque);

			inline reactphysics3d::RigidBody* get() { return this->rigid_body; };

		private:
			PhysicsWorld* physics_world;
			reactphysics3d::RigidBody* rigid_body;
		};

		class ProxyShape
		{
		public:
			ProxyShape(reactphysics3d::ProxyShape* shape_proxy);

			void setTransform(const TransformD& transform);
			TransformD getTransform();

			void setMass(double new_mass);
			double getMass();

			inline reactphysics3d::ProxyShape* get() { return this->shape_proxy; };

		private:
			reactphysics3d::ProxyShape* shape_proxy;
		};
	}
}