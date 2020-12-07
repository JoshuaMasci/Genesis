#pragma once

#include "Genesis/Scene/Ecs.hpp"
#include "Genesis/Scene/Entity.hpp"

namespace Genesis
{
	class ResourceManager;

	class EntityPropertiesWindow
	{
	protected:
		ResourceManager* resource_manager = nullptr;

	public:
		EntityPropertiesWindow(ResourceManager* resource_manager);

		void draw(Entity entity);
	};
}