#pragma once

#include "entt/entt.hpp"

namespace Genesis
{
	typedef entt::entity EntityHandle;
	typedef entt::basic_registry<EntityHandle> EntityRegistry;
	constexpr EntityHandle null_entity = entt::null;

	struct EntityNodeHandle
	{
		EntityNodeHandle(EntityHandle entity = null_entity, EntityHandle node = null_entity) : entity(entity), node(node) {};

		EntityHandle entity = null_entity;
		EntityHandle node = null_entity;

		bool operator== (const EntityNodeHandle &other)
		{
			return this->entity == other.entity && this->node == other.node;
		};
	};
}