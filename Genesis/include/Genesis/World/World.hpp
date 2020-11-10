#pragma once

#include "Genesis/World/ComponentSet.hpp"
#include "Genesis/World/Hierarchy.hpp"
#include "Genesis/World/Entity.hpp"

namespace Genesis
{
	class World
	{
	public:
		ComponentSet components;
		Hierarchy hierarchy;

		static void addEntity(World* world, Entity* entity)
		{
			world->hierarchy.children.insert(entity);
			entity->herarchy.parent = nullptr;
		};

		static void removeEntity(World* world, Entity* entity)
		{
			world->hierarchy.children.erase(entity);
			entity->herarchy.parent = nullptr;
		};
	};
}