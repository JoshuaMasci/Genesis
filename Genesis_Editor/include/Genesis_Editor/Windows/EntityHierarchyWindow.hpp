#pragma once

#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Scene/Entity.hpp"

#include "Genesis/World/GameWorld.hpp"
#include "Genesis/World/GameObject.hpp"

namespace Genesis
{
	class ResourceManager;

	class EntityHierarchyWindow
	{
	public:
		EntityHierarchyWindow(ResourceManager* resource_manager);
		void draw(Scene* scene);
		Entity getSelected();
		void drawEntity(Scene* scene, Entity entity);

		void draw(GameWorld* world);
		GameObject* getSelectedObject();
		void drawGameObject(GameObject* object);

	protected:
		Entity selected_entity;
		GameObject* selected_object = nullptr;

		GameObject* moved_object = nullptr;
		GameObject* target_object = nullptr;
		GameWorld* target_world = nullptr;
		
		GameObject* delete_object = nullptr;

		ResourceManager* resource_manager = nullptr;
	};
}