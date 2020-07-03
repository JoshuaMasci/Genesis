#pragma once

#include "Genesis/ECS/EntityRegistry.hpp"

namespace Genesis
{
	class EntityPropertiesWindow
	{
	public:
		EntityPropertiesWindow();
		~EntityPropertiesWindow();

		void drawWindow(EntityRegistry* registry, EntityWorld* world, EntityId selected_entity);
	};
}