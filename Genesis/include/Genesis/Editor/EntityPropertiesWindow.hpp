#pragma once

#include "Genesis/Entity/World.hpp"
#include "Genesis/Entity/Entity.hpp"

namespace Genesis
{
	class World;

	class EntityPropertiesWindow
	{
	public:
		EntityPropertiesWindow();
		~EntityPropertiesWindow();

		void drawWindow(World* world, Entity* selected_entity);
	};
}