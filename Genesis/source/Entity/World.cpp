#include "Genesis/Entity/World.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Debug/Profiler.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Core/Transform.hpp"
#include "Genesis/Entity/DebugCamera.hpp"

#include "Genesis/Physics/PhysicsSystem.hpp"
#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Entity/MeshComponent.hpp"

#include "Genesis/Entity/TransformSystem.hpp"

#include "Genesis/EntitySystem/EntitySystemScheduler.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

World::World(MeshPool* mesh_pool)
{
	this->mesh_pool = mesh_pool;
	this->world = new EntitySystem::EntityRegistry();
	this->physics_world = new PhysicsWorld(vector3D(0.0, -10.0, 0.0));

	this->world->registerComponent<WorldTransform>();
	this->world->registerComponent<Camera>();
	this->world->registerComponent<DebugCamera>();
	this->world->registerComponent<Physics::ProxyShape>();
	this->world->registerComponent<RigidBody>();
	this->world->registerComponent<MeshComponent>();


	//Scheduler Test
	{
		std::vector<EntitySignature> systems =
		{
			this->world->getSignature<WorldTransform>(),
			this->world->getSignature<WorldTransform, RigidBody>(),
			this->world->getSignature<Physics::ProxyShape>(),
			this->world->getSignature<WorldTransform, MeshComponent>()
		};

		EntitySystem::EntitySystemSceduler sceduler(systems);
	}


	EntityHandle entity = this->world->createEntity<WorldTransform, DebugCamera, Physics::ProxyShape>();
	this->world->initalizeComponent<WorldTransform>(entity, vector3D(0.0, 0.0, -20.0));
	WorldTransform* transform = this->world->getComponent<WorldTransform>(entity);

	{
		this->camera = this->world->createEntity<WorldTransform, Camera, DebugCamera, RigidBody, Physics::ProxyShape>();
		this->world->initalizeComponent<WorldTransform>(this->camera, vector3D(0.0, 5.0, -20.0));
		this->world->initalizeComponent<Camera>(this->camera, 77.0f);
		this->world->initalizeComponent<DebugCamera>(this->camera, 5.0, 0.5);
		this->world->initalizeComponent<RigidBody>(this->camera, this->physics_world, this->world->getComponent<WorldTransform>(this->camera)->current)->setType(RigidBodyType::Kinematic);
		this->world->initalizeComponent<Physics::ProxyShape>(this->camera, this->world->getComponent<RigidBody>(this->camera)->addCollisionShape(new reactphysics3d::SphereShape(2.0f), TransformD(), 1.0f));
	}

	{
		EntityHandle entity = this->world->createEntity<WorldTransform, RigidBody, Physics::ProxyShape, MeshComponent>();
		this->world->initalizeComponent<WorldTransform>(entity, vector3D((0.0, -10.0, 0.0)));
		this->world->initalizeComponent<RigidBody>(entity, this->physics_world, this->world->getComponent<WorldTransform>(entity)->current)->setType(RigidBodyType::Static);
		this->world->initalizeComponent<Physics::ProxyShape>(entity, this->world->getComponent<RigidBody>(entity)->addCollisionShape(new reactphysics3d::BoxShape(reactphysics3d::Vector3(16.0, 1.0, 16.0)), TransformD(), 1.0f));
		this->world->initalizeComponent<MeshComponent>(entity, "res/ground.obj", this->mesh_pool);
	}

	for (uint32_t i = 0; i < 100; i++)
	{
		EntityHandle entity = this->world->createEntity<WorldTransform, RigidBody, Physics::ProxyShape, MeshComponent>();
		this->world->initalizeComponent<WorldTransform>(entity, vector3D(0.0, 100.0 + (i * 3.0), 0.0));
		this->world->initalizeComponent<RigidBody>(entity, this->physics_world, this->world->getComponent<WorldTransform>(entity)->current);
		this->world->initalizeComponent<Physics::ProxyShape>(entity, this->world->getComponent<RigidBody>(entity)->addCollisionShape(new reactphysics3d::SphereShape(1.0f), TransformD(), 1.0f));
		this->world->initalizeComponent<MeshComponent>(entity, "res/sphere.obj", this->mesh_pool);
	}

}


World::~World()
{
	delete this->world;
	delete this->physics_world;
}

void World::runSimulation(Application* application, TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("World::runSimulation");

	//TransformSystem::preSimulation(*this->world, &application->job_system);

	//ENTITY MOVEMENT PHASE
	if (this->physics_world != nullptr)
	{
		PhysicsSystem::prePhysicsUpdate(*this->world, &application->job_system);
		this->physics_world->simulate(time_step);
		PhysicsSystem::postPhysicsUpdate(*this->world, &application->job_system);
	}

	GENESIS_PROFILE_BLOCK_START("DebugCamera::update");
	DebugCamera::update(&application->input_manager, *this->world->getComponent<DebugCamera>(this->camera), this->world->getComponent<WorldTransform>(this->camera)->current, time_step);
	GENESIS_PROFILE_BLOCK_END();

	//RESOLVE ENTITY TRANSFORMS
	//TransformSystem::calculateHierarchy(this->entity_registry, &application->job_system);

	//Other Game Logic

}
