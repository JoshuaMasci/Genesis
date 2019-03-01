#pragma once

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "Genesis/Core/Types.hpp"
#include "Genesis/Core/Transform.hpp"

namespace Genesis
{
	btVector3 toBtVec3(const vector3D& vec);
	vector3D toVec3(const btVector3& vec);

	btQuaternion toBtQuat(const quaternionD& quat);
	quaternionD toQuat(const btQuaternion& quat);

	btTransform toBtTransform(const Transform& trans);
	Transform toTransform(const btTransform& trans);

	struct MyClosestRayResultCallback : public btCollisionWorld::RayResultCallback
	{
		MyClosestRayResultCallback(const btVector3&	rayFromWorld, const btVector3& rayToWorld)
			:m_rayFromWorld(rayFromWorld),
			m_rayToWorld(rayToWorld)
		{
		}

		btVector3 m_rayFromWorld;//used to calculate hitPointWorld from hitFraction
		btVector3 m_rayToWorld;

		btVector3 m_hitNormalWorld;
		btVector3 m_hitPointWorld;

		int m_bodyId = -1;

		virtual	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			m_closestHitFraction = rayResult.m_hitFraction;
			m_collisionObject = rayResult.m_collisionObject;
			if (normalInWorldSpace)
			{
				m_hitNormalWorld = rayResult.m_hitNormalLocal;
			}
			else
			{
				///need to transform normal into worldspace
				m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
			}
			m_hitPointWorld.setInterpolate3(m_rayFromWorld, m_rayToWorld, rayResult.m_hitFraction);

			if (rayResult.m_localShapeInfo != nullptr && rayResult.m_collisionObject->getCollisionShape()->getShapeType() == COMPOUND_SHAPE_PROXYTYPE)
			{
				m_bodyId = rayResult.m_localShapeInfo->m_triangleIndex;
				//m_bodyId = rayResult.m_collisionObject->getUserIndex();
			}
			else
			{
				m_bodyId = -1;
			}

			return rayResult.m_hitFraction;
		}

	};

	struct MyClosestRayNotMeResultCallback : public btCollisionWorld::RayResultCallback
	{
		MyClosestRayNotMeResultCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld, btCollisionObject* me)
			:m_rayFromWorld(rayFromWorld),
			m_rayToWorld(rayToWorld)
		{
			m_me = me;
		}
		btCollisionObject* m_me;

		btVector3 m_rayFromWorld;//used to calculate hitPointWorld from hitFraction
		btVector3 m_rayToWorld;

		btVector3 m_hitNormalWorld;
		btVector3 m_hitPointWorld;

		int m_bodyId = -1;

		virtual	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			if (m_me != rayResult.m_collisionObject)
			{
				m_closestHitFraction = rayResult.m_hitFraction;
				m_collisionObject = rayResult.m_collisionObject;

				if (normalInWorldSpace)
				{
					m_hitNormalWorld = rayResult.m_hitNormalLocal;
				}
				else
				{
					///need to transform normal into worldspace
					m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
				}
				m_hitPointWorld.setInterpolate3(m_rayFromWorld, m_rayToWorld, rayResult.m_hitFraction);

				if (rayResult.m_localShapeInfo != nullptr)
				{
					m_bodyId = rayResult.m_localShapeInfo->m_shapePart;
				}
				else
				{
					m_bodyId = -1;
				}

				return rayResult.m_hitFraction;
			}
			else
			{
				return m_closestHitFraction;
			}
		}

	};
}