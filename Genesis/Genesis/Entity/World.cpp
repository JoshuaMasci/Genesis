#include "World.hpp"

#include "Genesis/Scene/Camera.hpp"
#include "Genesis/Core/Transform.hpp"
#include "Genesis/Entity/DebugCamera.hpp"

using namespace Genesis;

World::World()
{
	this->camera = this->entity_registry.create();
	this->entity_registry.assign<TransformD>(this->camera);
	this->entity_registry.assign<Camera>(this->camera, 77.0f);
	this->entity_registry.assign<DebugCamera>(this->camera, 0.5, 0.2);
}

World::~World()
{
}

void World::runSimulation(Application* application, double delta_time)
{
	DebugCamera::update(&application->input_manager, this->entity_registry.get<DebugCamera>(this->camera), this->entity_registry.get<TransformD>(this->camera), delta_time);
}
