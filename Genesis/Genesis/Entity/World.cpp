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

#include "Genesis/Ecs/EscWorld.hpp"
//#include "Genesis/Ecs/EntitySignatureTable.hpp"

using namespace Genesis;
using namespace Genesis::Physics;


World::World(MeshPool* mesh_pool)
{
	this->mesh_pool = mesh_pool;
	this->physics_world = new PhysicsWorld(vector3D(0.0, -10.0, 0.0));

	//TEST ECS
	{
		EcsWorld world;
		world.registerComponent<WorldTransform>();
		world.registerComponent<Camera>();
		world.registerComponent<DebugCamera>();
		world.registerComponent<ProxyShape>();
		world.registerComponent<RigidBody>();
		world.registerComponent<MeshComponent>();

		EntityHandle entity = world.createEntity<WorldTransform, DebugCamera, ProxyShape>();
		world.initalizeComponent<WorldTransform>(entity, vector3D(0.0, 0.0, -20.0));
		WorldTransform* transform = world.getComponent<WorldTransform>(entity);

		{
			EntityHandle camera = world.createEntity<WorldTransform, Camera, DebugCamera, RigidBody, ProxyShape>();
			world.initalizeComponent<WorldTransform>(camera, vector3D(0.0, 0.0, -20.0));
			world.initalizeComponent<Camera>(camera, 77.0f);
			world.initalizeComponent<DebugCamera>(camera, 5.0, 0.5);

			world.initalizeComponent<RigidBody>(camera, this->physics_world, world.getComponent<WorldTransform>(camera)->current)->setType(RigidBodyType::Kinematic);
			world.initalizeComponent<ProxyShape>(camera, world.getComponent<RigidBody>(camera)->addCollisionShape(new reactphysics3d::SphereShape(2.0f), TransformD(), 1.0f));
		}


		world.destroyEntity(entity);
	}


	/*{
		this->camera = this->entity_registry.create();
		this->entity_registry.assign<WorldTransform>(this->camera, vector3D(0.0, 0.0, -20.0));
		this->entity_registry.assign<Camera>(this->camera, 77.0f);
		this->entity_registry.assign<DebugCamera>(this->camera, 5.0, 0.5);

		this->entity_registry.assign<RigidBody>(this->camera, this->physics_world->addRigidBody(this->entity_registry.get<WorldTransform>(this->camera).current));
		this->entity_registry.assign<ProxyShape>(this->camera, this->entity_registry.get<RigidBody>(this->camera).addCollisionShape(new reactphysics3d::SphereShape(2.0f), TransformD(), 1.0f));
		this->entity_registry.get<RigidBody>(this->camera).setType(RigidBodyType::Kinematic);
	}*/

}

World::~World()
{
	/*auto rigid_body_view = this->entity_registry.view<RigidBody>();
	for (auto entity : rigid_body_view)
	{
		auto& rigid_body = rigid_body_view.get<RigidBody>(entity);
		this->physics_world->removeRigidBody(rigid_body.get());
	}*/

	delete this->physics_world;

	/*auto mesh_view = this->entity_registry.view<MeshComponent>();
	for (auto entity : mesh_view)
	{
		auto& mesh = mesh_view.get<MeshComponent>(entity);
		if (mesh.mesh != nullptr)
		{
			this->mesh_pool->freeResource(mesh.mesh_file);
		}
	}*/
}

void World::runSimulation(Application* application, TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("World::runSimulation");

	//TransformSystem::preSimulation(this->entity_registry, &application->job_system);

	//ENTITY MOVEMENT PHASE
	if (this->physics_world != nullptr)
	{
		//PhysicsSystem::prePhysicsUpdate(&this->entity_registry, &application->job_system);
		this->physics_world->simulate(time_step);
		//PhysicsSystem::postPhysicsUpdate(&this->entity_registry, &application->job_system);
	}

	GENESIS_PROFILE_BLOCK_START("DebugCamera::update");
	//DebugCamera::update(&application->input_manager, this->entity_registry.get<DebugCamera>(this->camera), this->entity_registry.get<WorldTransform>(this->camera).current, time_step);
	GENESIS_PROFILE_BLOCK_END();

	//RESOLVE ENTITY TRANSFORMS
	//TransformSystem::calculateHierarchy(this->entity_registry, &application->job_system);

	//Other Game Logic

}
