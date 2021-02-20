#pragma once

#include "Genesis/Scene/Ecs.hpp"
#include "Genesis/Scene/SceneComponents.hpp"

#include "Genesis/Rendering/SceneLightingSettings.hpp"
#include "Genesis/Rendering/SceneRenderList.hpp"

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