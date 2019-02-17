#pragma once

#include "Genesis/World/Entity.hpp"
#include "Genesis/World/ChildEntity.hpp"
#include "Genesis/World/RootEntity.hpp"

namespace Genesis
{
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager();

		RootEntity* createRootEntity();
		ChildEntity* createChildEntity();

		Entity* getEntity(EntityId id);
		void destroyEntity(EntityId id);

	private:

		map<EntityId, Entity*> entities;

		EntityId getNextId();
		EntityId next_id = 1;
	};
};