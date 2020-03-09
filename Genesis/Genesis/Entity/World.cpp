#include "World.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Debug/Profiler.hpp"

#include "Genesis/Scene/Camera.hpp"
#include "Genesis/Core/Transform.hpp"
#include "Genesis/Entity/DebugCamera.hpp"

#include "Genesis/Physics/PhysicsSystem.hpp"
#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Entity/MeshComponent.hpp"

#include "Genesis/Entity/TransformSystem.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

World::World(MeshPool* mesh_pool)
{
	this->mesh_pool = mesh_pool;

	this->physics_world = new PhysicsWorld(vector3D(0.0, -10.0, 0.0));

	{
		this->camera = this->entity_registry.create();
		this->entity_registry.assign<WorldTransform>(this->camera, vector3D(0.0, 0.0, -20.0));
		this->entity_registry.assign<Camera>(this->camera, 77.0f);
		this->entity_registry.assign<DebugCamera>(this->camera, 5.0, 0.5);

		this->entity_registry.assign<RigidBody>(this->camera, this->physics_world->addRigidBody(this->entity_registry.get<WorldTransform>(this->camera).current));
		this->entity_registry.assign<ProxyShape>(this->camera, this->entity_registry.get<RigidBody>(this->camera).addCollisionShape(new reactphysics3d::SphereShape(2.0f), TransformD(), 1.0f));
		this->entity_registry.get<RigidBody>(this->camera).setType(RigidBodyType::Kinematic);
	}

	{
		EntityId entity = this->entity_registry.create();

		this->entity_registry.assign<WorldTransform>(entity, vector3D(0.0, -10.0, 0.0));
		this->entity_registry.assign<RigidBody>(entity, this->physics_world->addRigidBody(this->entity_registry.get<WorldTransform>(entity).current));
		this->entity_registry.get<RigidBody>(entity).setType(RigidBodyType::Static);
		this->entity_registry.assign<ProxyShape>(entity, this->entity_registry.get<RigidBody>(entity).addCollisionShape(new reactphysics3d::BoxShape(reactphysics3d::Vector3(16.0, 1.0, 16.0)), TransformD(), 0.0f));

		this->entity_registry.assign<MeshComponent>(entity);
		auto& mesh_component = this->entity_registry.get<MeshComponent>(entity);
		mesh_component.mesh_file = "res/ground.obj";
		mesh_component.mesh = this->mesh_pool->getResource(mesh_component.mesh_file);
	}

	for(uint32_t i = 0; i < 100; i++)
	{
		EntityId entity = this->entity_registry.create();
		this->entity_registry.assign<WorldTransform>(entity, vector3D(0.0, 200.0 + (i * 3.0), 0.0));
		this->entity_registry.assign<RigidBody>(entity, this->physics_world->addRigidBody(this->entity_registry.get<WorldTransform>(entity).current));
		this->entity_registry.assign<ProxyShape>(entity, this->entity_registry.get<RigidBody>(entity).addCollisionShape(new reactphysics3d::SphereShape(1.0f), TransformD(), 1.0f));

		this->entity_registry.assign<MeshComponent>(entity);
		auto& mesh_component = this->entity_registry.get<MeshComponent>(entity);
		mesh_component.mesh_file = "res/sphere.obj";
		mesh_component.mesh = this->mesh_pool->getResource(mesh_component.mesh_file);
	}

	{
		EntityId entity = this->entity_registry.create();
		this->entity_registry.assign<WorldTransform>(entity, vector3D(0.0, 4.0, 0.0));
		this->entity_registry.assign<RigidBody>(entity, this->physics_world->addRigidBody(this->entity_registry.get<WorldTransform>(entity).current));
		this->entity_registry.assign<ProxyShape>(entity, this->entity_registry.get<RigidBody>(entity).addCollisionShape(new reactphysics3d::SphereShape(1.0f), TransformD(), 2.0f));
		this->entity_registry.assign<MeshComponent>(entity, "res/sphere.obj", this->mesh_pool->getResource("res/sphere.obj"));

		{
			EntityId child = this->entity_registry.create();
			this->entity_registry.assign<WorldTransform>(child);
			this->entity_registry.assign<ChildTransform>(child, vector3F(2.0f, 0.0f, 0.0f));
			this->entity_registry.assign<ChildNode>(child).root_parent = entity;
			this->entity_registry.assign<ProxyShape>(child, this->entity_registry.get<RigidBody>(entity).addCollisionShape(new reactphysics3d::SphereShape(1.0f), TransformD(vector3D(2.0, 0.0, 0.0)), 1.0f));
			this->entity_registry.assign<MeshComponent>(child, "res/sphere.obj", this->mesh_pool->getResource("res/sphere.obj"));

			this->entity_registry.assign<ParentNode>(entity).child_entities.push_back(child);
		}

	}

}

World::~World()
{
	auto rigid_body_view = this->entity_registry.view<RigidBody>();
	for (auto entity : rigid_body_view)
	{
		auto& rigid_body = rigid_body_view.get<RigidBody>(entity);
		this->physics_world->removeRigidBody(rigid_body.get());
	}

	delete this->physics_world;

	auto mesh_view = this->entity_registry.view<MeshComponent>();
	for (auto entity : mesh_view)
	{
		auto& mesh = mesh_view.get<MeshComponent>(entity);
		if (mesh.mesh != nullptr)
		{
			this->mesh_pool->freeResource(mesh.mesh_file);
		}
	}
}

void World::runSimulation(Application* application, TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("World::runSimulation");

	TransformSystem::preSimulation(this->entity_registry, &application->job_system);

	//ENTITY MOVEMENT PHASE
	if (this->physics_world != nullptr)
	{
		PhysicsSystem::prePhysicsUpdate(&this->entity_registry, &application->job_system);
		this->physics_world->simulate(time_step);
		PhysicsSystem::postPhysicsUpdate(&this->entity_registry, &application->job_system);
	}

	GENESIS_PROFILE_BLOCK_START("DebugCamera::update");
	DebugCamera::update(&application->input_manager, this->entity_registry.get<DebugCamera>(this->camera), this->entity_registry.get<WorldTransform>(this->camera).current, time_step);
	GENESIS_PROFILE_BLOCK_END();

	//RESOLVE ENTITY TRANSFORMS
	TransformSystem::calculateHierarchy(this->entity_registry, &application->job_system);

	//Other Game Logic

}
