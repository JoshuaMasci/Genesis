#pragma once

#include "Genesis/Ecs/Entt.hpp"

namespace Genesis
{
	struct ParentNode
	{
		vector<EntityHandle> child_nodes;
	};

	struct ChildNode
	{
		EntityHandle parent_node;
		TransformF local_transform;
	};
}