#pragma once

#include "genesis_engine/scene/Ecs.hpp"
#include "genesis_engine/scene/SceneComponents.hpp"

#include "genesis_engine/rendering/SceneLightingSettings.hpp"
#include "genesis_engine/rendering/SceneRenderList.hpp"

namespace genesis_engine
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

		SceneLightingSettings lighting_settings;
		SceneRenderList render_list;

		// TODO
		//EntityWorld* clone();

		//TODO figure out to do this better
		void initialize_scene();
		void deinitialize_scene();

		friend class Entity;
	};
}