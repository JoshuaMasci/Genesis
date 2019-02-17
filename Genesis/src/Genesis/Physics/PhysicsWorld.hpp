#pragma once

#include "Genesis/Physics/ReactPhysics3D_Include.hpp"

namespace Genesis
{
	class SingleRayCallback : public reactphysics3d::RaycastCallback
	{
	public:
		bool has_hit = false;

		reactphysics3d::decimal smallest_hit_fraction = 1.1;
		reactphysics3d::CollisionBody* hit_body = nullptr;
		reactphysics3d::ProxyShape* hit_shape = nullptr;
		reactphysics3d::Vector3 world_point;
		reactphysics3d::Vector3 world_normal;

		reactphysics3d::decimal notifyRaycastHit(const  reactphysics3d::RaycastInfo& raycastInfo)
		{
			if (raycastInfo.hitFraction < this->smallest_hit_fraction)
			{
				this->has_hit = true;
				this->hit_body = raycastInfo.body;
				this->hit_shape = raycastInfo.proxyShape;
				this->world_point = raycastInfo.worldPoint;
				this->world_normal = raycastInfo.worldNormal;
				this->smallest_hit_fraction = raycastInfo.hitFraction;
			}

			return 1.0;
		};
	};

	class PhysicsWorld
	{

	public:
		PhysicsWorld();
		virtual ~PhysicsWorld();

		void update(double delta_time);

	protected:
		reactphysics3d::DynamicsWorld* dynamics_world = nullptr;
	};
};