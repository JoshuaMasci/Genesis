#pragma once

#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Scene/Entity.hpp"

namespace Genesis
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