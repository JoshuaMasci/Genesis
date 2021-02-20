#pragma once

#include "genesis_engine/scene/Scene.hpp"
#include "genesis_engine/scene/Entity.hpp"

namespace genesis_engine
{
	class ResourceManager;

	class EntityHierarchyWindow
	{
	public:
		EntityHierarchyWindow(ResourceManager* resource_manager);
		void draw(Scene* scene);
		Entity get_selected();

	protected:
		void drawEntity(Scene* scene, Entity entity);

		Entity selected_entity;

		ResourceManager* resource_manager = nullptr;
	};
}