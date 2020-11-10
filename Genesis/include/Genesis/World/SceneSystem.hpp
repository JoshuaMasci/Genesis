#pragma once

#include "Genesis/World/Entity.hpp"
#include "Genesis/World/World.hpp"
#include "Genesis/World/WorldSimulator.hpp"

#include "Genesis/Rendering/SceneInfo.hpp"

namespace Genesis
{
	struct SceneSystem : public WorldSystem
	{
		SceneSystem()
		{
			this->pre_update = updateWorld;
			this->entity_update = updateEntity;
		};

		static void updateWorld(const TimeStep time_step, World* world)
		{
			SceneInfo* scene = world->components.get<SceneInfo>();
			if (scene)
			{
				scene->clearBuffers();
			}
		};

		static void updateEntity(const TimeStep time_step, World* world, Entity* root, const TransformD& root_transform, Entity* entity)
		{
			SceneInfo* scene = world->components.get<SceneInfo>();
			if (scene)
			{
				TransformD world_transfrom;
				TransformUtils::transformByInplace(world_transfrom, root->local_transform, root_transform);

				/*Camera* camera_component = entity->components.get<Camera>();
				if (camera_component)
				{
					scene->cameras.push_back({ *camera_component, world_transfrom });
				}*/

				ModelComponent* model_component = entity->components.get<ModelComponent>();
				if (model_component)
				{
					scene->models.push_back({ model_component->mesh , model_component->material, world_transfrom });
				}

				DirectionalLight* directional_component = entity->components.get<DirectionalLight>();
				if (directional_component)
				{
					scene->directional_lights.push_back({ *directional_component, world_transfrom });
				}

				PointLight* point_component = entity->components.get<PointLight>();
				if (point_component)
				{
					scene->point_lights.push_back({ *point_component, world_transfrom });
				}

			}
		};
	};
}