#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	class EntityHierarchyWindow
	{
	public:
		EntityHierarchyWindow();
		~EntityHierarchyWindow();

		void draw(EntityRegistry& registry);

		EntityHandle getSelected() { return this->selected_entity; };
	private:

		void drawEntityTree(EntityRegistry& registry, EntityHandle entity);

		EntityHandle selected_entity = null_entity;
	};
}