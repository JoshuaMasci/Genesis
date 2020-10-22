#pragma once

#include "Genesis/Ecs/Ecs.hpp"

namespace Genesis
{
	class Entity
	{
	public:
		Entity(EntityRegistry* registry = nullptr, EntityHandle entity = null_entity)
		{
			this->registry = registry;
			this->entity_handle = entity;
		}

		bool valid()
		{
			return this->registry != nullptr && this->registry->valid(this->entity_handle);
		}

		EntityHandle getHandle()
		{
			return this->entity_handle;
		}

		EntityRegistry* getRegistry()
		{
			return this->registry;
		}

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			GENESIS_ENGINE_ASSERT(this->registry != nullptr, "Registry is nullptr");
			GENESIS_ENGINE_ASSERT(!this->hasComponent<T>(), "Entity already has component!");
			return this->registry->assign<T>(entity_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent()
		{
			GENESIS_ENGINE_ASSERT(this->registry != nullptr, "Registry is nullptr");
			GENESIS_ENGINE_ASSERT(this->hasComponent<T>(), "Entity does not have component!");
			return this->registry->get<T>(entity_handle);
		}

		template<typename T>
		bool hasComponent()
		{
			GENESIS_ENGINE_ASSERT(this->registry != nullptr, "Registry is nullptr");
			return this->registry->has<T>(entity_handle);
		}

		template<typename T>
		void removeComponent()
		{
			GENESIS_ENGINE_ASSERT(this->registry != nullptr, "Registry is nullptr");
			GENESIS_ENGINE_ASSERT(this->hasComponent<T>(), "Entity does not have component!");
			this->registry->remove<T>(entity_handle);
		}

		bool operator==(const Entity& other)
		{
			return this->entity_handle == other.entity_handle && this->registry == other.registry;
		};

		bool operator!=(const Entity& other)
		{
			return !this->operator==(other);
		};

	protected:
		EntityHandle entity_handle = null_entity;
		EntityRegistry* registry = nullptr;
	};
}