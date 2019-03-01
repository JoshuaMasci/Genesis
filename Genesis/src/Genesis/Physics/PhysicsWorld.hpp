#pragma once

#include "Genesis/Physics/Bullet_Include.hpp"

namespace Genesis
{
	//Prototypes
	class Entity;
	class RigidBody;

	struct SingleRayTestResult
	{
		bool hasHit = false;

		Entity* entity = nullptr;

		vector3D hitPosition;
		vector3D hitNormal;
	};

	class PhysicsWorld
	{

	public:
		PhysicsWorld();
		virtual ~PhysicsWorld();

		virtual void update(double delta_time);

		void addRigidBody(RigidBody* rigidBody);
		void removeRigidBody(RigidBody* rigidBody);

		SingleRayTestResult singleRayTest(vector3D startPos, vector3D endPos);
		SingleRayTestResult singleRayTestNotMe(vector3D startPos, vector3D endPos, Entity* me);
		btDiscreteDynamicsWorld* dynamicsWorld = nullptr;

	protected:
		//TODO: Remove this list
		set<RigidBody*> rigid_bodies;

		btBroadphaseInterface* broadphase = nullptr;
		btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
		btCollisionDispatcher* dispatcher = nullptr;
		btSequentialImpulseConstraintSolver* solver = nullptr;
	};
}
