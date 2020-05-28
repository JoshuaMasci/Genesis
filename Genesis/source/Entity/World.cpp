#include "Genesis/Entity/World.hpp"

#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Ecs/DebugCamera.hpp"
#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

#include "Genesis/LegacyRendering/LegacyMeshRenderer.hpp"

#include "Genesis/Physics/CollisionShape.hpp"

using namespace Genesis;

World::World(BaseWorldRenderer* world_renderer)
{
	this->world_renderer = world_renderer;

	this->entity_registry = new EntityRegistry();
	this->physics_world = new PhysicsWorld(vector3D(0.0, -9.8, 0.0));

	{
		this->main_camera = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(this->main_camera, vector3D(0.0, 0.0, -20.0));
		this->entity_registry->assign<Camera>(this->main_camera, 77.0f);
		this->entity_registry->assign<DebugCamera>(this->main_camera, 5.0, 0.3);
	}
}

World::~World()
{
	for (Entity* entity : this->root_entities)
	{
		entity->removeFromWorld();
		delete entity;
	}

	delete this->entity_registry;
	delete this->physics_world;
}

void World::runSimulation(Application* application, TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("World::runSimulation");
	physics_world->simulate(time_step);

	DebugCamera::update(application->input_manager, this->entity_registry->get<DebugCamera>(this->main_camera), this->entity_registry->get<TransformD>(this->main_camera), time_step);

	for (Entity* entity : this->root_entities)
	{
		entity->updateTransform();
	}
}

void World::addEntity(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "world tried to add a null child");
	GENESIS_ENGINE_ASSERT_ERROR(entity->parent == nullptr && entity->world == nullptr, ("{}:{} already has a parent", entity->id, entity->name));
	
	entity->addtoWorld(this);
	this->root_entities.push_back(entity);
}

void World::removeEntity(Entity* entity)
{
	GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "world tried to remove a null entity");

	for (size_t i = 0; i < this->root_entities.size(); i++)
	{
		if (this->root_entities[i] == entity)
		{
			size_t last_index = this->root_entities.size() - 1;
			if (i != last_index)
			{
				this->root_entities[i] = this->root_entities[last_index];
			}
			this->root_entities.pop_back();
			entity->removeFromWorld();
			return;
		}
	}

	GENESIS_ENGINE_ERROR("{}:{} is not in world", entity->id, entity->name);
}
