#pragma once

#include "entt/entt.hpp"

namespace Genesis
{
	typedef entt::entity EntityHandle;
	typedef entt::basic_registry<EntityHandle> EntityRegistry;
	constexpr EntityHandle null_entity = entt::null;

	typedef entt::entity NodeHandle;
	typedef entt::basic_registry<EntityHandle> NodeRegistry;
	constexpr NodeHandle null_node = entt::null;

	struct EntityNodeHandle
	{
		EntityNodeHandle(EntityHandle entity = null_entity, NodeHandle node = null_node) : entity(entity), node(node) {};

		EntityHandle entity = null_entity;
		NodeHandle node = null_node;

		bool operator== (const EntityNodeHandle &other)
		{
			return this->entity == other.entity && this->node == other.node;
		};
	};
}