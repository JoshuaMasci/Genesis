#include "World.hpp"

#include "Genesis/Debug/Assert.hpp"

#include "Genesis/Scene/Camera.hpp"
#include "Genesis/Core/Transform.hpp"
#include "Genesis/Entity/DebugCamera.hpp"

#include "Genesis/Physics/PhysicsSystem.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

World::World()
{
	
}

World::~World()
{

}

void World::runSimulation(Application* application, TimeStep time_step)
{
	if (this->physics_world != nullptr)
	{
		PhysicsSystem::prePhysicsUpdate(&this->entity_registry, &application->job_system);
		this->physics_world->world->update((reactphysics3d::decimal)time_step);
		PhysicsSystem::postPhysicsUpdate(&this->entity_registry, &application->job_system);
	}

	DebugCamera::update(&application->input_manager, this->entity_registry.get<DebugCamera>(this->camera), this->entity_registry.get<TransformD>(this->camera), time_step);
}
