#pragma once

#include "Genesis/Scene/Ecs.hpp"

namespace Genesis
{
	class SceneComponents
	{
	public:
		SceneComponents(EntityRegistry* entity_registry = nullptr, EntityHandle entity = null_entity)
		{
			this->entity_registry = entity_registry;
			this->entity_handle = entity;
		}

		EntityHandle handle()
		{
			return this->entity_handle;
		}

		template<typename T, typename... Args>
		T& add(Args&&... args)
		{
			GENESIS_ENGINE_ASSERT(!this->has<T>(), "Scene already has component!");
			return this->entity_registry->assign<T>(entity_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& get()
		{
			GENESIS_ENGINE_ASSERT(this->has<T>(), "Scene doesn't have component!");
			return this->entity_registry->get<T>(entity_handle);
		}

		template<typename T>
		bool has()
		{
			return this->entity_registry->has<T>(entity_handle);
		}

		template<typename T>
		void remove()
		{
			GENESIS_ENGINE_ASSERT(this->has<T>(), "Scene doesn't have component!");
			this->entity_registry->remove<T>(entity_handle);
		}

	protected:
		EntityRegistry* entity_registry;
		EntityHandle entity_handle;
	};
}