#include "Genesis/World/World.hpp"

#include "Genesis/World/Entity.hpp"

namespace Genesis
{
	namespace Experimental
	{
		World::World()
		{
		}

		World::~World()
		{
			for (auto component_ptr : this->components)
			{
				delete component_ptr.second;
			}

			for (auto entity : this->root_entities)
			{
				delete entity;
			}
		}

		void World::setEntityParent(Entity* entity)
		{
			this->entity_parent = entity;
		}

		Entity* World::getEntityParent()
		{
			return this->entity_parent;
		}

		void World::addEntity(Entity* entity)
		{
			this->root_entities.insert(entity);
		}

		void World::removeEntity(Entity* entity)
		{
			this->root_entities.erase(entity);
		}
	}
}