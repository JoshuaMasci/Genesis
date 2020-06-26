#include "Genesis/Entity/EntityManager.hpp"

namespace Genesis
{
	EntityManager::EntityManager()
	{
	}

	EntityManager::~EntityManager()
	{
	}

	Entity* EntityManager::createEntity()
	{
		EntityId entity_id = this->next_id;
		this->next_id++;
		return new Entity(entity_id);
	}

	void EntityManager::destroyEntity(Entity* entity)
	{
		GENESIS_ENGINE_ASSERT_ERROR(has_value(this->entites, entity->getId()), "Entity doesn't exsit in manager");

		//TODO Mark for delete
		this->entites.erase(entity->getId());
		delete entity;
	}
}