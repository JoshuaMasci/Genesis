#pragma once

#include "Genesis/Entity/EntitySystem.hpp"
#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	class World;

	class BaseWorldRenderer
	{
	public:
		virtual void addMesh(EntityRegistry* registry, EntityHandle entity, const string& mesh_file, const string& material_file) = 0;
		virtual void removeEntity(EntityRegistry* registry, EntityHandle entity) = 0;

		virtual void drawWorld(World* world, vector2U size) = 0;
	};
}