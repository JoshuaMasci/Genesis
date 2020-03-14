#include "PhysicsSystem.hpp"

#include "Genesis/Debug/Profiler.hpp"
#include "Genesis/Debug/Log.hpp"

#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"
#include "Genesis/Entity/TransformSystem.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

void PhysicsSystem::prePhysicsUpdate(EntityRegistry* registry, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::prePhysicsUpdate");

	auto view = registry->view<RigidBody, WorldTransform>();

	for (auto entity : view)
	{
		RigidBody& rigid_body = view.get<RigidBody>(entity);
		WorldTransform& world_transform = view.get<WorldTransform>(entity);
		rigid_body.setTransform(world_transform.current);
	}
}

void PhysicsSystem::postPhysicsUpdate(EntityRegistry* registry, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::postPhysicsUpdate");

	auto view = registry->view<RigidBody, WorldTransform>();

	for (auto entity : view)
	{
		RigidBody& rigid_body = view.get<RigidBody>(entity);
		WorldTransform& world_transform = view.get<WorldTransform>(entity);
		TransformD rigid_body_transform = rigid_body.getTransform();
		world_transform.current.setPosition(rigid_body_transform.getPosition());
		world_transform.current.setOrientation(rigid_body_transform.getOrientation());
	}
}
