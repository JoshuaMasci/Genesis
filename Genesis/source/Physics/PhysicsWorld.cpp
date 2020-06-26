#include "Genesis/Physics/PhysicsWorld.hpp"

#include "Genesis/Physics/Rigidbody.hpp"

#include "Genesis/Entity/Entity.hpp"

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

	void PhysicsWorld::addEntity(Entity* entity)
	{
		GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "Physics World tried to add a null entity");
		GENESIS_ENGINE_ASSERT_ERROR(entity->getRigidbody() != nullptr, "Physics World tried to add a null rigidbody");

		TransformD& transform = entity->getWorldTransform();
		reactphysics3d::Transform react_transform;
		react_transform.setPosition(toVec3R(transform.getPosition()));
		react_transform.setOrientation(toQuatR(transform.getOrientation()));

		RigidBody* rigidbody = entity->getRigidbody();
		rigidbody->setHandle(this->dynamics_world->createRigidBody(react_transform));
	}

	void PhysicsWorld::removeEntity(Entity* entity)
	{
		GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "Physics World tried to remove a null entity");
		GENESIS_ENGINE_ASSERT_ERROR(entity->getRigidbody() != nullptr, "Physics World tried to remove a null rigidbody");

		RigidBody* rigidbody = entity->getRigidbody();
		reactphysics3d::RigidBody* react_rigidbody = rigidbody->getHandle();
		this->dynamics_world->destroyRigidBody(react_rigidbody);
		rigidbody->setHandle(nullptr);
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

	Node* PhysicsWorld::castRay(vector3D start_position, vector3D end_pos)
	{
		reactphysics3d::Ray ray(toVec3R(start_position), toVec3R(end_pos));
		SingleRayCallback callback;
		this->dynamics_world->raycast(ray, &callback);

		if (callback.has_hit)
		{
			return (Node*)callback.hit_shape->getUserData();
		}

		return nullptr;
	}
}