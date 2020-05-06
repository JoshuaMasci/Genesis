#pragma once

#include "Genesis/Entity/EntitySystem.hpp"

namespace Genesis
{
	class World;

	class BaseWorldRenderer
	{
	public:
		virtual void drawWorld(World* world, vector2U size) = 0;
	};
}