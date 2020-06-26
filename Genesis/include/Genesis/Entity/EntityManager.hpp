#pragma once

#include "Genesis/Entity/Entity.hpp"

namespace Genesis
{
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager();

		Entity* createEntity();
		void destroyEntity(Entity* entity);

	protected:
		EntityId next_id = 0;
		map<EntityId, Entity*> entites;
	};
}