#pragma once

#include "Genesis/Entity/World.hpp"
#include "Genesis/Entity/Entity.hpp"

namespace Genesis
{
	class World;

	class HierarchyWindow
	{
	public:
		HierarchyWindow();
		~HierarchyWindow();

		void drawWindow(World* world);

		Entity* getSelectedEntity() { return this->selected_entity; };

	protected:
		void drawEntityTree(Entity* entity);
		Entity* selected_entity = nullptr;
	};
}