#include "Genesis/Physics/PhysicsSystem.hpp"

#include "Genesis/Debug/Profiler.hpp"
#include "Genesis/Debug/Log.hpp"

#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"
#include "Genesis/Entity/TransformSystem.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

void prePhysicsUpdateThread(uint32_t thread_id, EntitySystem::EntityView view)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::prePhysicsUpdateThread");

	for (size_t i = 0; i < view.getSize(); i++)
	{
		WorldTransform* world_transform = view.getComponent<WorldTransform>(i);
		RigidBody* rigid_body = view.getComponent<RigidBody>(i);
		rigid_body->setTransform(world_transform->current);
	}
}

void PhysicsSystem::prePhysicsUpdate(EntitySystem::EntityRegistry& world, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::prePhysicsUpdate");

	/*vector<View> views = world.getView<WorldTransform, RigidBody>();
	for (auto& view : views)
	{
		for (size_t i = 0; i < view.getSize(); i++)
		{
			WorldTransform* world_transform = view.getComponent<WorldTransform>(i);
			RigidBody* rigid_body = view.getComponent<RigidBody>(i);
			rigid_body->setTransform(world_transform->current);
		}
	}*/

	JobCounter counter = 0;

	vector<EntitySystem::EntityView> views = world.getView<WorldTransform, RigidBody>();
	for (auto& view : views)
	{
		job_system->addJob(std::bind(prePhysicsUpdateThread, std::placeholders::_1, view), &counter);
	}

	JobSystem::waitForCounter(counter);
}

void PhysicsSystem::postPhysicsUpdate(EntitySystem::EntityRegistry& world, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::postPhysicsUpdate");

	vector<EntitySystem::EntityView> views = world.getView<WorldTransform, RigidBody>();
	for (auto& view : views)
	{
		for (size_t i = 0; i < view.getSize(); i++)
		{
			WorldTransform* world_transform = view.getComponent<WorldTransform>(i);
			RigidBody* rigid_body = view.getComponent<RigidBody>(i);
			TransformD rigid_body_transform = rigid_body->getTransform();
			world_transform->current.setPosition(rigid_body_transform.getPosition());
			world_transform->current.setOrientation(rigid_body_transform.getOrientation());
		}
	}
}
