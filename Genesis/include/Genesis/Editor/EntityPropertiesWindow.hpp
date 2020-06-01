#pragma once

#include "Genesis/Ecs/EcsSystem.hpp"

namespace Genesis
{
	class World;

	class EntityPropertiesWindow
	{
	public:
		EntityPropertiesWindow();
		~EntityPropertiesWindow();

		void drawWindow(EcsWorld* world, EntityHandle selected_entity);
	};
}