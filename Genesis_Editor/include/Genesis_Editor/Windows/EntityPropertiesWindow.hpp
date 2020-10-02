#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	//Asset Pools Forward Declaration
	class MeshPool;
	class MaterialPool;

	class EntityPropertiesWindow
	{
	protected:
		//Asset Pools
		MeshPool* mesh_pool = nullptr;
		MaterialPool* material_pool = nullptr;

	public:
		EntityPropertiesWindow(MeshPool* mesh_pool, MaterialPool* material_pool);
		~EntityPropertiesWindow();

		void draw(EntityRegistry& registry, EntityHandle selected_entity);
	};
}