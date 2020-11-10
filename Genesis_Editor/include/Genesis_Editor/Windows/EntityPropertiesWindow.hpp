#pragma once

#include "Genesis/World/Entity.hpp"

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

		void draw(Entity* entity);
	};
}