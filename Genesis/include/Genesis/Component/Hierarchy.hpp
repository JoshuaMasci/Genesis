#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	struct Hierarchy
	{
		static void addChild(EntityRegistry& registry, EntityHandle parent, EntityHandle child);
		static void removeChild(EntityRegistry& registry, EntityHandle parent, EntityHandle child);
	};

	struct ParentNode
	{
		EntityHandle first{ null_entity };
	};

	struct ChildNode
	{
		EntityHandle prev{ null_entity };
		EntityHandle next{ null_entity };
		EntityHandle parent{ null_entity };
	};
}