#pragma once

#include "Genesis/Entity/EntitySystem.hpp"

namespace Genesis
{
	class World;

	class BaseWorldRenderer
	{
	public:
		virtual void addEntity(EntityRegistry* registry, EntityHandle entity, const string& mesh_file, const string& material_file) = 0;
		virtual void removeEntity(EntityRegistry* registry, EntityHandle entity) = 0;

		virtual void drawWorld(World* world) = 0;
	};
}