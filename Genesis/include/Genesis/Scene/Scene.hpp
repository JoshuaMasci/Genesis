#pragma once

#include "Genesis/Scene/Ecs.hpp"
#include "Genesis/Scene/SceneComponents.hpp"

namespace Genesis
{
	class Entity;

	class Scene
	{
	public:
		EntityRegistry registry;
		SceneComponents scene_components;

		Scene();

		Entity createEntity(const char* name);
		void destoryEntity(Entity entity);

		void addChild(Entity parent, Entity child);
		void removeChild(Entity parent, Entity child);

		// TODO
		//EntityWorld* clone();

		friend class Entity;
	};
}