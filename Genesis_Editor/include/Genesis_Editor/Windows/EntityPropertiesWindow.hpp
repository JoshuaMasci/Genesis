#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	class EntityPropertiesWindow
	{
	public:
		EntityPropertiesWindow();
		~EntityPropertiesWindow();

		void drawWindow(EntityRegisty& world, EntityHandle selected_entity);
	};
}