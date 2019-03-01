#include "PhysicsWorld.hpp"

#include "Genesis/Physics/RigidBody.hpp"

using namespace Genesis;

PhysicsWorld::PhysicsWorld()
{
	// Build the broadphase
	broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver();

	// The world
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0.0, 0.0, 0.0));
}

PhysicsWorld::~PhysicsWorld()
{
	// Clean up after ourselves like good little programmers
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}

void PhysicsWorld::update(double delta_time)
{
	//Run Physics Simulation
	this->dynamicsWorld->stepSimulation(delta_time, 8, 1.0 / 240.0);
}

void PhysicsWorld::addRigidBody(RigidBody* rigidBody)
{
	/*this->dynamicsWorld->addRigidBody(rigidBody->getRigidBody());
	rigidBody->setPhysicsWorld(this);*/
	this->rigid_bodies.insert(rigidBody);
}

void PhysicsWorld::removeRigidBody(RigidBody* rigidBody)
{
	/*this->dynamicsWorld->removeRigidBody(rigidBody->getRigidBody());
	rigidBody->setPhysicsWorld(nullptr);*/
	this->rigid_bodies.erase(rigidBody);
}

SingleRayTestResult PhysicsWorld::singleRayTest(vector3D startPos, vector3D endPos)
{
	btVector3 start = toBtVec3(startPos);
	btVector3 end = toBtVec3(endPos);

	MyClosestRayResultCallback rayCallback(start, end);

	// Perform raycast
	dynamicsWorld->rayTest(start, end, rayCallback);

	SingleRayTestResult result;

	if (rayCallback.hasHit())
	{
		result.hasHit = true;
		const btRigidBody* hitBody = btRigidBody::upcast(rayCallback.m_collisionObject);

		result.hitPosition = toVec3(rayCallback.m_hitPointWorld);
		result.hitNormal = toVec3(rayCallback.m_hitNormalWorld);

		result.entity = (Entity*)hitBody->getUserPointer();

		if (rayCallback.m_bodyId != -1)
		{
			if (hitBody->getCollisionShape()->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
			{
				btCompoundShape* shape = (btCompoundShape*)hitBody->getCollisionShape();

				btCollisionShape* child = nullptr;
				
				if (rayCallback.m_bodyId <= shape->getNumChildShapes())
				{
					child = shape->getChildShape(rayCallback.m_bodyId);
				}

			}
		}
	}

	return result;
}

SingleRayTestResult PhysicsWorld::singleRayTestNotMe(vector3D startPos, vector3D endPos, Entity* me)
{
	/*btVector3 start = toBtVec3(startPos);
	btVector3 end = toBtVec3(endPos);

	btCollisionWorld::AllHitsRayResultCallback rayCallback(start, end);

	// Perform raycast
	dynamicsWorld->rayTest(start, end, rayCallback);

	SingleRayTestResult result;

	if (rayCallback.hasHit())
	{
		int closestHitIndex = -1;

		for (int i = 0; i < rayCallback.m_collisionObjects.size(); i++)
		{
			if (rayCallback.m_collisionObjects[i]->getUserPointer() != me)
			{
				if (closestHitIndex != -1)
				{
					btVector3 distance1 = rayCallback.m_hitPointWorld[i] - start;
					btVector3 distance2 = rayCallback.m_hitPointWorld[closestHitIndex] - start;
					if (distance1.length() < distance2.length())
					{
						closestHitIndex = i;
					}
				}
				else
				{
					closestHitIndex = i;
				}
			}

		}

		if (closestHitIndex != -1)
		{
			result.hasHit = true;
			const btRigidBody* hitBody = btRigidBody::upcast(rayCallback.m_collisionObjects[closestHitIndex]);

			result.hitPosition = toVec3(rayCallback.m_hitPointWorld[closestHitIndex]);
			result.hitNormal = toVec3(rayCallback.m_hitNormalWorld[closestHitIndex]);
			result.entity = (Entity*)hitBody->getUserPointer();
		}

	}

	return result;*/
	return SingleRayTestResult();
}