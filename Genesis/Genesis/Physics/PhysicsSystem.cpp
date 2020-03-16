#include "PhysicsSystem.hpp"

#include "Genesis/Debug/Profiler.hpp"
#include "Genesis/Debug/Log.hpp"

#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"
#include "Genesis/Entity/TransformSystem.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

void PhysicsSystem::prePhysicsUpdate(EcsWorld& world, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::prePhysicsUpdate");

	vector<View> views = world.getView<WorldTransform, RigidBody>();
	for (auto& view : views)
	{
		for (size_t i = 0; i < view.getSize(); i++)
		{
			EntityHandle entity = view.get(i);
			WorldTransform* world_transform = view.getComponent<WorldTransform>(entity);
			RigidBody* rigid_body = view.getComponent<RigidBody>(entity);
			rigid_body->setTransform(world_transform->current);
		}
	}
}

void PhysicsSystem::postPhysicsUpdate(EcsWorld& world, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::postPhysicsUpdate");

	vector<View> views = world.getView<WorldTransform, RigidBody>();
	for (auto& view : views)
	{
		for (size_t i = 0; i < view.getSize(); i++)
		{
			EntityHandle entity = view.get(i);
			WorldTransform* world_transform = view.getComponent<WorldTransform>(entity);
			RigidBody* rigid_body = view.getComponent<RigidBody>(entity);
			TransformD rigid_body_transform = rigid_body->getTransform();
			world_transform->current.setPosition(rigid_body_transform.getPosition());
			world_transform->current.setOrientation(rigid_body_transform.getOrientation());
		}
	}
}
