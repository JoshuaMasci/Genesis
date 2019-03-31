#include "PhysicsWorld.hpp"

#include "Genesis/Physics/RigidBody.hpp"

using namespace Genesis;

PhysicsWorld::PhysicsWorld()
{
	// Build the broadphase
	broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	collision_configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_configuration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver();

	// The world
	dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0.0, 0.0, 0.0));
}

PhysicsWorld::~PhysicsWorld()
{
	// Clean up after ourselves like good little programmers
	delete dynamics_world;
	delete solver;
	delete dispatcher;
	delete collision_configuration;
	delete broadphase;
}

void PhysicsWorld::runSimulation(double delta_time)
{
	//Run Physics Simulation
	this->dynamics_world->stepSimulation(delta_time, 8, 1.0 / 240.0);

	//TODO update forces on each rigid body every tick
}

void PhysicsWorld::addRigidBody(RigidBody* rigidBody)
{
	this->dynamics_world->addRigidBody(rigidBody->getRigidBody());
	this->rigid_bodies.insert(rigidBody);
}

void PhysicsWorld::removeRigidBody(RigidBody* rigidBody)
{
	this->dynamics_world->removeRigidBody(rigidBody->getRigidBody());
	this->rigid_bodies.erase(rigidBody);
}

SingleRayTestResult PhysicsWorld::singleRayTest(vector3D startPos, vector3D endPos)
{
	btVector3 start = toBtVec3(startPos);
	btVector3 end = toBtVec3(endPos);

	MyClosestRayResultCallback rayCallback(start, end);

	// Perform raycast
	dynamics_world->rayTest(start, end, rayCallback);

	SingleRayTestResult result;

	if (rayCallback.hasHit())
	{
		result.hasHit = true;
		const btRigidBody* hitBody = btRigidBody::upcast(rayCallback.m_collisionObject);

		result.hitPosition = toVec3(rayCallback.m_hitPointWorld);
		result.hitNormal = toVec3(rayCallback.m_hitNormalWorld);

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