#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	class Entity
	{
	protected:
		EntityHandle entity_handle = null_entity;
		EntityRegistry* registry = nullptr;

	public:
		Entity(EntityHandle entity, EntityRegistry* registry)
		{
			this->entity_handle = entity;
			this->registry = registry;
		}

		bool valid()
		{
			return this->registry->valid(this->entity_handle);
		}

		EntityHandle getHandle()
		{
			return this->entity_handle;
		}

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			GENESIS_ENGINE_ASSERT(!this->hasComponent<T>(), "Entity already has component!");
			return this->registry->assign<T>(entity_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent()
		{
			GENESIS_ENGINE_ASSERT(this->hasComponent<T>(), "Entity does not have component!");
			return this->registry->get<T>(entity_handle);
		}

		template<typename T>
		bool hasComponent()
		{
			return this->registry->has<T>(entity_handle);
		}

		template<typename T>
		void removeComponent()
		{
			GENESIS_ENGINE_ASSERT(this->hasComponent<T>(), "Entity does not have component!");
			this->registry->remove<T>(entity_handle);
		}
	};
}