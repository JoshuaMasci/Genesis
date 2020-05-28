#include "Genesis/Physics/PhysicsSystem.hpp"

#include "Genesis/Physics/Rigidbody.hpp"

using namespace Genesis;

void PhyscisSystem::update(EcsWorld* world, TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("PhyscisSystem::update");

	auto& view = world->entity_registry.view<RigidBody, TransformD>();
	for (EntityHandle entity : view)
	{
		RigidBody& rigid_body = view.get<RigidBody>(entity);
		TransformD& transform = view.get<TransformD>(entity);
		rigid_body.setTransform(transform);
	}

	world->physics_world->simulate(time_step);

	for (EntityHandle entity : view)
	{
		RigidBody& rigid_body = view.get<RigidBody>(entity);
		TransformD& transform = view.get<TransformD>(entity);
		TransformD rigid_body_transform = rigid_body.getTransform();
		transform.setPosition(rigid_body_transform.getPosition());
		transform.setOrientation(rigid_body_transform.getOrientation());
	}
}
