#pragma once

#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Scene/Scene.hpp"
#include "Genesis/Rendering/SceneInfo.hpp"

namespace Genesis
{
	class SceneSystem
	{
	public:
		static void build_scene(Scene* scene)
		{
			SceneInfo* scene_info = &scene->scene_components.get<SceneInfo>();
			EntityRegistry& registry = scene->registry;

			scene_info->clear_buffers();

			auto model_group = registry.view<ModelComponent, WorldTransform>();
			for (EntityHandle entity : model_group)
			{
				ModelComponent& model_component = model_group.get<ModelComponent>(entity);
				WorldTransform& transform = model_group.get<WorldTransform>(entity);

				if (model_component.mesh != nullptr && model_component.material != nullptr)
				{
					scene_info->models.push_back({ model_component.mesh , model_component.material, transform.getTransform() });
				}
			}

			auto directional_light_group = registry.view<DirectionalLight, WorldTransform>();
			for (EntityHandle entity : directional_light_group)
			{
				auto&[light, transform] = directional_light_group.get<DirectionalLight, WorldTransform>(entity);
				scene_info->directional_lights.push_back({ light, transform.getTransform() });
			}

			auto point_light_group = registry.view<PointLight, WorldTransform>();
			for (EntityHandle entity : point_light_group)
			{
				auto&[light, transform] = point_light_group.get<PointLight, WorldTransform>(entity);
				scene_info->point_lights.push_back({ light, transform.getTransform() });
			}
		};
	};
}