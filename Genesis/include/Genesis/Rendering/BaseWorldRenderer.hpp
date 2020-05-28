#pragma once

#include "Genesis/Ecs/Entt.hpp"

namespace Genesis
{
	class World;

	class BaseWorldRenderer
	{
	public:
		virtual void drawWorld(World* world, vector2U size) = 0;
	};
}