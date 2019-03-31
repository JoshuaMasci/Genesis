#include "PhysicsSystem.hpp"

#include "PhysicsUpdateJob.hpp"

using namespace Genesis;

PhysicsSystem::PhysicsSystem()
{
	this->physics_world = new PhysicsWorld();
}

PhysicsSystem::~PhysicsSystem()
{
	delete this->physics_world;
}

void PhysicsSystem::runSimulation(EntityRegistry& registry, JobSystem& job_system, double delta_time)
{
	//Pre Physics
	this->PrePhysicsUpdate(registry, job_system, delta_time);

	//Physics Update
	this->PhysicsUpdate(registry, job_system, delta_time);

	//Post Physics
	this->PostPhysicsUpdate(registry, job_system, delta_time);
}

void PhysicsSystem::PrePhysicsUpdate(EntityRegistry& registry, JobSystem& job_system, double delta_time)
{
	auto view = registry.view<WorldTransform, RigidBody>();

	for (auto entity : view)
	{
		auto& transform = view.get<WorldTransform>(entity);
		auto& rigid_body = view.get<RigidBody>(entity);

		if (rigid_body.getWorld() == nullptr)
		{
			rigid_body.addToWorld(this->physics_world);
		}

		rigid_body.setWorldTransform(transform.current_transform);
	}
}

void PhysicsSystem::PhysicsUpdate(EntityRegistry& registry, JobSystem& job_system, double delta_time)
{
	this->physics_world->runSimulation(delta_time);
}

void PhysicsSystem::PostPhysicsUpdate(EntityRegistry& registry, JobSystem& job_system, double delta_time)
{
	auto view = registry.view<WorldTransform, RigidBody>();

	for (auto entity : view)
	{
		auto &transform = view.get<WorldTransform>(entity);
		auto &rigid_body = view.get<RigidBody>(entity);

		transform.current_transform = rigid_body.getWorldTransform();
	}
}
