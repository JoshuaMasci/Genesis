#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	struct Hierarchy
	{
		EntityHandle first{ null_entity };
		EntityHandle prev{ null_entity };
		EntityHandle next{ null_entity };
		EntityHandle parent{ null_entity };

		static void addChild(EntityRegisty& registry, EntityHandle parent, EntityHandle child);
		static void removeChild(EntityRegisty& registry, EntityHandle parent, EntityHandle child);
	};
}