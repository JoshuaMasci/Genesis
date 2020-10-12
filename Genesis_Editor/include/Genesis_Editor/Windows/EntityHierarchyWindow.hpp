#pragma once

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/Entity.hpp"
#include "Genesis/Ecs/EntityWorld.hpp"

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

		EntityHandle getSelected() { return this->selected_entity; };

	private:
		void drawEntityTree(Entity entity);

		EntityHandle selected_entity = null_entity;
	};
}