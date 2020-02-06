#include "PhysicsSystem.hpp"

#include "Genesis/Debug/Profiler.hpp"

#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"
#include "Genesis/Core/Transform.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

void PhysicsSystem::prePhysicsUpdate(EntityRegistry* registry, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::prePhysicsUpdate()");
	auto view = registry->view<RigidBody, TransformD>();

	for (auto entity : view)
	{
		auto& rigid_body = view.get<RigidBody>(entity);
		auto& transform_d = view.get<TransformD>(entity);

		if (rigid_body.rigid_body != nullptr)
		{
			reactphysics3d::Transform transform_r = rigid_body.rigid_body->getTransform();
			transform_r.setPosition(toVec3R(transform_d.getPosition()));
			transform_r.setOrientation(toQuatR(transform_d.getOrientation()));
			rigid_body.rigid_body->setTransform(transform_r);
		}
	}
}

void PhysicsSystem::postPhysicsUpdate(EntityRegistry* registry, JobSystem* job_system)
{
	GENESIS_PROFILE_FUNCTION("PhysicsSystem::postPhysicsUpdate()");
	auto view = registry->view<RigidBody, TransformD>();

	for (auto entity : view)
	{
		auto& rigid_body = view.get<RigidBody>(entity);
		auto& transform_d = view.get<TransformD>(entity);

		if (rigid_body.rigid_body != nullptr)
		{
			const reactphysics3d::Transform& transform_r = rigid_body.rigid_body->getTransform();
			transform_d.setPosition(toVec3D(transform_r.getPosition()));
			transform_d.setOrientation(toQuatD(transform_r.getOrientation()));
		}
	}
}
