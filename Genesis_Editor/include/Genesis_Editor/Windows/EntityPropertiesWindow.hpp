#pragma once

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/Entity.hpp"

namespace Genesis
{
	//Asset Pools Forward Declaration
	class MeshPool;
	class MaterialPool;

	class EntityPropertiesWindow
	{
	protected:
	public:
		void draw(Entity entity, MeshPool* mesh_pool, MaterialPool* material_pool);
	};
}