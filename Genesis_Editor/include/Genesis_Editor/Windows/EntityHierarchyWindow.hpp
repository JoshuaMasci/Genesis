#pragma once

#include "Genesis/Ecs/EntityWorld.hpp"
#include "Genesis/Ecs/Entity.hpp"

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
		Entity getSelected();

	private:
		void drawEntity(Entity entity);

		Entity selected_entity;
	};
}