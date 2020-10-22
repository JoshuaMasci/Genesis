#pragma once

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/Entity.hpp"
#include "Genesis/Ecs/EntityWorld.hpp"

#include "Genesis/Component/NodeComponent.hpp"

namespace Genesis
{
	class MeshPool;
	class MaterialPool;

	class EntityHierarchyWindow
	{
	public:
		EntityHierarchyWindow();
		~EntityHierarchyWindow();

		void draw(EntityWorld* world, MeshPool* mesh_pool, MaterialPool* material_pool);

		Entity getSelected() { return this->selected_entity; };

	private:
		void drawEntity(Entity entity);
		void drawEntityNode(Entity entity_node);

		Entity selected_entity;
	};
}