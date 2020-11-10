#pragma once

#include "Genesis/World/World.hpp"
#include "Genesis/World/Entity.hpp"

namespace Genesis
{
	class MeshPool;
	class MaterialPool;

	class EntityHierarchyWindow
	{
	public:
		EntityHierarchyWindow();
		~EntityHierarchyWindow();

		void draw(World* world, MeshPool* mesh_pool, MaterialPool* material_pool);
		Entity* getSelected();

	private:
		void drawEntity(World* world, Entity* entity);

		Entity* selected_entity = nullptr;
	};
}