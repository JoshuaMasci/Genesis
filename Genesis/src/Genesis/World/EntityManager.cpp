#include "EntityManager.hpp"

using namespace Genesis;

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
	for (auto entity : this->entities)
	{
		delete entity.second;
	}
}

RootEntity* EntityManager::createRootEntity()
{
	RootEntity* entity = new RootEntity(this, this->getNextId());
	this->entities[entity->entityId] = entity;
	return entity;
}

ChildEntity* EntityManager::createChildEntity()
{
	ChildEntity* entity = new ChildEntity(this, this->getNextId());
	this->entities[entity->entityId] = entity;
	return entity;
}

Entity* EntityManager::getEntity(EntityId id)
{
	if (this->entities.find(id) == this->entities.end())
	{
		return nullptr;
	}

	return this->entities[id];
}

void EntityManager::destroyEntity(EntityId id)
{
}

EntityId EntityManager::getNextId()
{
	EntityId entity_id = this->next_id;

	/*while (this->entities.find(entity_id) != this->entities.end())
	{
		this->next_id++;
		entity_id = this->next_id;
	}*/

	this->next_id++;
	return entity_id;
}
