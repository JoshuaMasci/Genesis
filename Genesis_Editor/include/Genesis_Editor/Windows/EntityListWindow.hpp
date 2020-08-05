#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	class EntityListWindow
	{
	public:
		EntityListWindow();
		~EntityListWindow();

		void drawWindow(EntityRegisty& registry);

		EntityHandle getSelected() { return this->selected_entity; };
	private:

		void drawEntityTree(EntityRegisty& registry, EntityHandle entity);

		EntityHandle selected_entity = null_entity;
	};
}