#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	class EntityPropertiesWindow
	{
	public:
		EntityPropertiesWindow();
		~EntityPropertiesWindow();

		void draw(EntityRegistry& world, EntityHandle selected_entity);
	};
}