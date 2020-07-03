#pragma once

#include "Genesis/ECS/EntityRegistry.hpp"

namespace Genesis
{
	class WorldViewWindow
	{
	public:
		WorldViewWindow();
		~WorldViewWindow();
		void drawWindow(EntityRegistry* registry, EntityWorld* world);

		EntityId getSelected() { return this->selected_entity; };
	private:

		EntityId selected_entity = InvalidEntity;
	};
}