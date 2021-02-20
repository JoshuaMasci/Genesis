#pragma once

#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Scene/Ecs.hpp"
#include "Genesis/Scene/Hierarchy.hpp"
#include "Genesis/System/EntitySystem.hpp"

namespace Genesis
{
	class TransformResolveSystem: public EntitySystem
	{
	public:
		virtual void run(Scene* scene, const TimeStep time_step)
		{
			EntityRegistry& registry = scene->registry;
			auto view = registry.view<Transform>(entt::exclude_t<ChildNode>());
			for (auto entity : view)
			{
				Transforms transforms;

				TransformD local_transform = registry.get<Transform>(entity);
				transforms.root_transform.setScale(local_transform.getScale()); //Root transform only gets the scale of the root object
				TransformUtils::transformByInplace(transforms.world_transform, transforms.world_transform, local_transform);

				if (registry.has<RootTransform>(entity))
				{
					registry.get<RootTransform>(entity).setTransform(transforms.root_transform);
				}

				if (registry.has<WorldTransform>(entity))
				{
					registry.get<WorldTransform>(entity).setTransform(transforms.world_transform);
				}

				for (EntityHandle child : EntityHiearchy(&registry, entity))
				{
					this->resolveTransformChild(registry, child, transforms);
				}
			}
		};

	private:
		struct Transforms
		{
			Transform world_transform;
			Transform root_transform;
		};

		void resolveTransformChild(EntityRegistry& registry, EntityHandle entity, Transforms transforms)
		{
			if (registry.has<Transform>(entity))
			{
				Transform local_transform = registry.get<Transform>(entity);
				TransformUtils::transformByInplace(transforms.root_transform, transforms.root_transform, local_transform);
				TransformUtils::transformByInplace(transforms.world_transform, transforms.world_transform, local_transform);
			}

			if (registry.has<RootTransform>(entity))
			{
				registry.get<RootTransform>(entity).setTransform(transforms.root_transform);
			}

			if (registry.has<WorldTransform>(entity))
			{
				registry.get<WorldTransform>(entity).setTransform(transforms.world_transform);
			}

			for (EntityHandle child : EntityHiearchy(&registry, entity))
			{
				this->resolveTransformChild(registry, child, transforms);
			}
		}
	};
}