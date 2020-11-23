#pragma once

#pragma once

#include "Genesis/Ecs/Ecs.hpp"

#include "Genesis/Ecs/Hierarchy.hpp"

namespace Genesis
{
	class Entity
	{
	public:
		Entity(EntityRegistry* entity_registry = nullptr, EntityHandle entity = null_entity)
		{
			this->entity_registry = entity_registry;
			this->entity_handle = entity;
		}

		bool valid()
		{
			if (!this->entity_registry)
				return false;

			return this->entity_registry->valid(this->entity_handle);
		}

		EntityHandle handle()
		{
			return this->entity_handle;
		}

		EntityRegistry* registry()
		{
			return this->entity_registry;
		}

		template<typename T, typename... Args>
		T& add(Args&&... args)
		{
			GENESIS_ENGINE_ASSERT(!this->has<T>(), "Entity already has component!");
			return this->entity_registry->assign<T>(entity_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& get()
		{
			GENESIS_ENGINE_ASSERT(this->has<T>(), "Entity does not have component!");
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
			GENESIS_ENGINE_ASSERT(this->has<T>(), "Entity does not have component!");
			this->entity_registry->remove<T>(entity_handle);
		}

		void add_child(Entity entity)
		{
			HierarchyUtils::addChild(*this->entity_registry, this->entity_handle, entity.entity_handle);
		}

		void remove_child(Entity entity)
		{
			HierarchyUtils::removeChild(*this->entity_registry, this->entity_handle, entity.entity_handle);
		}

		EntityHiearchy children()
		{
			return EntityHiearchy(this->entity_registry, this->entity_handle);
		}
		
		bool operator==(const Entity& entity)
		{
			return (this->entity_registry == entity.entity_registry) && (this->entity_handle == entity.entity_handle);
		}

		bool operator!=(const Entity& entity)
		{
			return (this->entity_registry != entity.entity_registry) || (this->entity_handle != entity.entity_handle);
		}

	protected:
		EntityRegistry* entity_registry = nullptr;
		EntityHandle entity_handle = null_entity;
	};
}