#pragma once

#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Scene/Entity.hpp"

namespace Genesis
{
	class MeshPool;
	class MaterialPool;

	class EntityHierarchyWindow
	{
	public:
		EntityHierarchyWindow();
		~EntityHierarchyWindow();

		void draw(Scene* scene, MeshPool* mesh_pool, MaterialPool* material_pool);
		Entity getSelected();

	private:
		void drawEntity(Scene* scene, Entity entity);

		Entity selected_entity;
	};
}