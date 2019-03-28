#pragma once

#include "Genesis/Physics/Bullet_Include.hpp"

namespace Genesis
{
	//Prototypes
	class RigidBody;

	struct SingleRayTestResult
	{
		bool hasHit = false;

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

	protected:
		btDiscreteDynamicsWorld* dynamics_world = nullptr;

		//TODO: find better container;
		set<RigidBody*> rigid_bodies;

		btBroadphaseInterface* broadphase = nullptr;
		btDefaultCollisionConfiguration* collision_configuration = nullptr;
		btCollisionDispatcher* dispatcher = nullptr;
		btSequentialImpulseConstraintSolver* solver = nullptr;
	};
}
