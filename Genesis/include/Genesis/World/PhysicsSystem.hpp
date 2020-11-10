#pragma once

#include "Genesis/World/Entity.hpp"
#include "Genesis/World/World.hpp"
#include "Genesis/World/WorldSimulator.hpp"

#include "Genesis/Physics/PhysicsWorld.hpp"
#include "Genesis/Physics/RigidBody.hpp"

namespace Genesis
{
	struct PhysicsSystem : public WorldSystem
	{
		PhysicsSystem()
		{
			this->pre_update = preUpdate;
			this->post_update = PostUpdate;
		};

		static void preUpdate(const TimeStep time_step, World* world)
		{
			for (Entity* root : world->hierarchy.children)
			{
				RigidBody* rigid_body = root->components.get<RigidBody>();
				if (rigid_body)
				{
					rigid_body->setTransform(root->local_transform);
				}
			}

			PhysicsWorld* physics_world = world->components.get<PhysicsWorld>();
			if (physics_world)
			{
				physics_world->simulate(time_step);
			}
		};

		static void PostUpdate(const TimeStep time_step, World* world)
		{
			for (Entity* root : world->hierarchy.children)
			{
				RigidBody* rigid_body = root->components.get<RigidBody>();
				if (rigid_body)
				{
					rigid_body->getTransform(root->local_transform);
				}
			}
		};
	};
}