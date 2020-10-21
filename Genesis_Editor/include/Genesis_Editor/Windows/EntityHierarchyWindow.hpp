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

		EntityNodeHandle getSelected() { return this->selected_node; };

	private:
		void drawEntity(EntityWorld* world, Entity entity);
		void drawEntityNode(NodeComponent& node_component, EntityNodeHandle entity_node);

		EntityNodeHandle selected_node = {};
	};
}