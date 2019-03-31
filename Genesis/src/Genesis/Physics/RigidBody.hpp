#pragma once

#include "Genesis/Physics/Bullet_Include.hpp"

namespace Genesis
{
	//Prototype Class
	class PhysicsWorld;

	class RigidBody
	{
	public:
		RigidBody();
		virtual ~RigidBody();

		Transform getWorldTransform();
		void setWorldTransform(const Transform& transform);

		void setCollisionShape(btCollisionShape* shape);

		inline btRigidBody* getRigidBody() { return this->rigid_body; };

		void addToWorld(PhysicsWorld* world);
		void removeFromWorld();
		inline PhysicsWorld* getWorld() { return this->physics_world; };

	protected:

		btRigidBody* rigid_body = nullptr;

		PhysicsWorld* physics_world = nullptr;

		btEmptyShape* empty_shape = nullptr;
	};
}