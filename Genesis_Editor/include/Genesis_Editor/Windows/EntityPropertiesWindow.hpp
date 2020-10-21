#pragma once

#include "Genesis/Ecs/Ecs.hpp"
#include "Genesis/Ecs/Entity.hpp"
#include "Genesis/Ecs/EntityWorld.hpp"
#include "Genesis/Component/NodeComponent.hpp"

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

		void drawEntity(EntityWorld* entity_world, EntityHandle entity_handle);
		void drawNode(EntityWorld* entity_world, EntityNodeHandle entity_node_handle);

	public:
		EntityPropertiesWindow(MeshPool* mesh_pool, MaterialPool* material_pool);
		~EntityPropertiesWindow();

		void draw(EntityWorld* entity_world, EntityNodeHandle selected_entity_node);
	};
}