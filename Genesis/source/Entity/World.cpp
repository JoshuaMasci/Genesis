#include "Genesis/Entity/World.hpp"

#include "Genesis/Entity/Entity.hpp"

#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

namespace Genesis
{
	World::World()
	{
		this->physics_world = new PhysicsWorld(vector3D(0.0, 0.0, 0.0));
	}

	World::~World()
	{
		for (Entity* entity : this->entities)
		{
			entity->removeFromWorld();
			delete entity;
		}

		delete this->physics_world;
	}

	void World::runSimulation(Application* application, TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("World::runSimulation");
		physics_world->simulate(time_step);

		for (Entity* entity : this->entities)
		{
			entity->onUpdate(time_step);
		}
	}

	void World::addEntity(Entity* entity)
	{
		GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "world tried to add a null child");
		GENESIS_ENGINE_ASSERT_ERROR(entity->world == nullptr, ("{}:{} already in a world", entity->id, entity->name));

		entity->addtoWorld(this);
		this->entities.push_back(entity);
	}

	void World::removeEntity(Entity* entity)
	{
		GENESIS_ENGINE_ASSERT_ERROR(entity != nullptr, "world tried to remove a null entity");

		for (size_t i = 0; i < this->entities.size(); i++)
		{
			if (this->entities[i] == entity)
			{
				size_t last_index = this->entities.size() - 1;
				if (i != last_index)
				{
					this->entities[i] = this->entities[last_index];
				}
				this->entities.pop_back();
				entity->removeFromWorld();
				return;
			}
		}

		GENESIS_ENGINE_ERROR("{}:{} is not in world", entity->id, entity->name);
	}
}