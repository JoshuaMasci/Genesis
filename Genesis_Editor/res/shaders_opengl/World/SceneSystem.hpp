#pragma once

#include "Genesis/World/Entity.hpp"
#include "Genesis/World/World.hpp"
#include "Genesis/World/WorldSimulator.hpp"

#include "Genesis/Rendering/SceneInfo.hpp"

namespace Genesis
{
	namespace Experimental
	{
		struct SceneSystem
		{
			static void updateWorld(const TimeStep time_step, World* world)
			{
				SceneInfo* scene = world->getComponent<SceneInfo>();
				if (scene)
				{
					scene->clearBuffers();
				}
			};

			static void updateEntity(const TimeStep time_step, World* world, Entity* root, const TransformF& root_transform, Entity* entity)
			{
				SceneInfo* scene = world->getComponent<SceneInfo>();
				if (scene)
				{
					TransformD world_transfrom;

					ModelComponent* model_component = entity->getComponent<ModelComponent>();
					if (model_component)
					{
						scene->models.push_back({ model_component.mesh , model_component.material, world_transfrom });
					}

				}
			};


		};
	}
}