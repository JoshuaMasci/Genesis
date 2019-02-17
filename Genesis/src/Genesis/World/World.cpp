#include "World.hpp"

#include "Genesis/World/RootEntity.hpp"

using namespace Genesis;

World::World()
{
}

World::~World()
{
	for (int i = 0; i < this->root_entities_in_world.size(); i++)
	{
		if (this->root_entities_in_world[i] == nullptr)
		{
			delete this->root_entities_in_world[i];//TODO use delete system
			return;
		}
	}
}

void World::addGameObjectToWorld(RootEntity* entity)
{
	if(entity->getWorld() == nullptr)
	{
		auto it = find(this->root_entities_in_world.begin(), this->root_entities_in_world.end(), entity);
		if (it == this->root_entities_in_world.end())
		{
			for (int i = 0; i < this->root_entities_in_world.size(); i++)
			{
				if (this->root_entities_in_world[i] == nullptr)
				{
					this->root_entities_in_world[i] = entity;
					return;
				}
			}

			//No empty spots
			this->root_entities_in_world.push_back(entity);
		}
	}
	else
	{
		printf("Error: Object can't be added, already in world\n");
	}
}

void World::removeGameObjectFromWorld(RootEntity* entity)
{
	for (int i = 0; i < this->root_entities_in_world.size(); i++)
	{
		if (this->root_entities_in_world[i] == entity)
		{
			this->root_entities_in_world[i] = nullptr;
			return;
		}
	}
}

void World::updatePreFrame(double delta_time)
{
}

void World::updateTick(double delta_time)
{
}

void World::updatePostFrame(double delta_time)
{
}
