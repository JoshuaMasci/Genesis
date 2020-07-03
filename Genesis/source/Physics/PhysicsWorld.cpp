#include "Genesis/Physics/PhysicsWorld.hpp"

namespace Genesis
{
	PhysicsWorld::PhysicsWorld(vector3D gravity)
	{
		this->dynamics_world = new reactphysics3d::DynamicsWorld(toVec3R(gravity));
	}

	PhysicsWorld::~PhysicsWorld()
	{
		delete this->dynamics_world;
	}

	void PhysicsWorld::simulate(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("PhysicsWorld::simulate");
		this->dynamics_world->update((reactphysics3d::decimal)time_step);
	}

	class SingleRayCallback : public reactphysics3d::RaycastCallback
	{
	public:
		bool has_hit = false;

		reactphysics3d::decimal smallest_hit_fraction = 1.1;
		reactphysics3d::CollisionBody* hit_body = nullptr;
		reactphysics3d::ProxyShape* hit_shape = nullptr;
		reactphysics3d::Vector3 world_point;
		reactphysics3d::Vector3 world_normal;

		reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& raycastInfo)
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

	void PhysicsWorld::castRay(vector3D start_position, vector3D end_pos)
	{
		reactphysics3d::Ray ray(toVec3R(start_position), toVec3R(end_pos));
		SingleRayCallback callback;
		this->dynamics_world->raycast(ray, &callback);

		return;
	}
}