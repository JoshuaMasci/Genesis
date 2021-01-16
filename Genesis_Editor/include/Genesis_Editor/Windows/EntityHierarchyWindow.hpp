#pragma once

#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Scene/Entity.hpp"

namespace Genesis
{
	class ResourceManager;
	class EntityPrototype;
	class ScenePrototype;

	class EntityHierarchyWindow
	{
	public:
		EntityHierarchyWindow(ResourceManager* resource_manager);

		void draw(ScenePrototype* scene);
		EntityPrototype* get_selected();

	protected:
		ResourceManager* resource_manager = nullptr;

		void draw_entity(EntityPrototype* entity);
		EntityPrototype* selected_entity = nullptr;

		EntityPrototype* moved_entity = nullptr;
		EntityPrototype* target_entity = nullptr;
		ScenePrototype* target_scene = nullptr;
		EntityPrototype* delete_entity = nullptr;
	};
}