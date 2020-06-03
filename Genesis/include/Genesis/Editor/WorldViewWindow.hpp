#pragma once

#include "Genesis/Entity/World.hpp"

namespace Genesis
{
	class WorldViewWindow
	{
	public:
		WorldViewWindow();
		~WorldViewWindow();
		void drawWindow(World* world);
		Entity* selected_entity_ptr = nullptr;
	private:
		void drawEntity(Entity* entity);
	};
}