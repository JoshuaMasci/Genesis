#pragma once

#include "Genesis/World/World.hpp"

namespace Genesis
{
	class WorldManager
	{
	public:
		WorldManager();
		~WorldManager();

		World* createWorld();
		void destroyWorld(World* world);

		void update(TimeStep time_step);

	protected:
		WorldId next_id = 1;

		map<WorldId, World*> worlds;
	};
}