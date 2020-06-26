#include "Genesis/World/WorldManager.hpp"

namespace Genesis
{
	WorldManager::WorldManager()
	{
	}

	WorldManager::~WorldManager()
	{
		for (auto world : this->worlds)
		{
			delete world.second;
		}
	}

	World* WorldManager::createWorld()
	{
		WorldId world_id = this->next_id;
		this->next_id++;
		return new World(world_id);
	}

	void WorldManager::destroyWorld(World* world)
	{
		GENESIS_ENGINE_ASSERT_ERROR(has_value(this->worlds, world->getId()), "World doesn't exsit in manager");

		//TODO Mark for delete
		this->worlds.erase(world->getId());
		delete world;
	}

	void WorldManager::update(TimeStep time_step)
	{
		for (auto world : this->worlds)
		{
			world.second->update(time_step);
		}
	}
}