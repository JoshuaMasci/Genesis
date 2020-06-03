#pragma once

#include "Genesis/Physics/ReactPhyscis.hpp"

namespace Genesis
{
	class RigidBody
	{
	public:
		RigidBody();
		~RigidBody();

		inline bool inWorld() { return this->rigidbody != nullptr; };

		void setStatic() { this->rigidbody->setType(reactphysics3d::BodyType::STATIC); };

		reactphysics3d::ProxyShape* addCollisionShape(reactphysics3d::CollisionShape* collision_shape, const TransformF& transform);
		void removeCollisionShape(reactphysics3d::ProxyShape* proxy_shape);

		void setTransform(const TransformD& transform);
		TransformD getTransform();

		void setLinearVelocity(const vector3D& velocity);
		vector3D getLinearVelocity();

		void setAngularVelocity(const vector3D& velocity);
		vector3D getAngularVelocity();

		void setGravityEnabled(bool enabled);
		bool getGravityEnabled();

		void setAwake(bool awake);
		bool getAwake();

		void applyCenteredForce(const vector3D& force);
		void applyForce(const vector3D& world_position, const vector3D& force);
		void applyTorque(const vector3D& torque);

		inline void setHandle(reactphysics3d::RigidBody* rigidbody_handle) { this->rigidbody = rigidbody_handle; };
		inline reactphysics3d::RigidBody* getHandle() { return this->rigidbody; };

	private:
		reactphysics3d::RigidBody* rigidbody = nullptr;
	};

	/*class CollisionShape
	{
	public:
		virtual void updateTransform(const TransformD& root_transform) = 0;

		inline reactphysics3d::CollisionShape* getCollisionShape() { return this->collision_shape; };

		void setProxyShape(reactphysics3d::ProxyShape* proxy_shape) { this->proxy_shape = proxy_shape; };
		reactphysics3d::ProxyShape* getProxyShape() { return this->proxy_shape; };

	protected:
		TransformD current_transform;
		reactphysics3d::CollisionShape* collision_shape = nullptr;
		reactphysics3d::ProxyShape* proxy_shape = nullptr;
	};

	class BoxCollisionShape : CollisionShape
	{
	public:
		BoxCollisionShape(vector3D size);
		~BoxCollisionShape();
		virtual void updateTransform(const TransformD & root_transform) override;
	};*/
}