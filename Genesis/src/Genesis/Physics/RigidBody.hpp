#pragma once

#include "Genesis/Physics/Bullet_Include.hpp"

namespace Genesis
{
	//Prototype Class
	class Entity;
	class PhysicsWorld;

	class RigidBody
	{
	public:
		RigidBody(Entity* entity);
		virtual ~RigidBody();

		void setMass(double mass);
		double getMass();

		void setInertiaTensor(vector3D inertia);
		vector3D getInertiaTensor();

		void Activate(bool activate);

		Transform getWorldTransform();
		void setWorldTransform(Transform transform);

		vector3D getLinearVelocity() const;
		void setLinearVelocity(vector3D velocity);

		vector3D getAngularVelocity() const;
		void setAngularVelocity(vector3D velocity);

		void applyForce(vector3D &force, vector3D &localPos);
		void applyImpulse(vector3D &impulse, vector3D &localPos);

		void applyCentralForce(vector3D &force);
		void applyCentralImpulse(vector3D &impulse);

		void applyTorque(vector3D &torque);
		void applyTorqueImpulse(vector3D &torque);

		void setDampening(double linear, double angular);

		bool isInWorld();

		inline btRigidBody* getRigidBody() { return this->rigidBody; };

		inline PhysicsWorld* getPhysicsWorld() { return this->physics_world; };
		inline void setPhysicsWorld(PhysicsWorld* world) { this->physics_world = world; };

	protected:
		void setCollisionShape(btCollisionShape* shape);

		double mass = 1.0;

		btRigidBody* rigidBody = nullptr;

		Entity* parent = nullptr;

		PhysicsWorld* physics_world = nullptr;
	};
}