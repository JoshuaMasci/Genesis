#pragma once

#include "Genesis/Ecs/EcsWorld.hpp"
#include "Genesis/Entity/World.hpp"

namespace Genesis
{
	class HierarchyWindow
	{
	public:
		HierarchyWindow();
		~HierarchyWindow();

		void drawWindow(EcsWorld* world);
		void drawWindow(World* world);

		EntityHandle selected_entity = entt::null;
		Entity* selected_entity_ptr = nullptr;
	private:
		void drawEntityHierarchy(EntityRegistry& registry, EntityHandle entity);
		void drawEntity(Entity* entity);
	};
}