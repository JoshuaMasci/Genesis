#pragma once

#include "genesis_engine/scene/Scene.hpp"
#include "genesis_engine/scene/Hierarchy.hpp"

namespace Genesis
{
	class Entity
	{
	protected:
		Scene* scene = nullptr;
		EntityHandle entity_handle = null_entity;

	public:
		Entity(Scene* scene = nullptr, EntityHandle entity = null_entity)
		{
			this->scene = scene;
			this->entity_handle = entity;
		}

		template<typename T, typename... Args>
		T& add(Args&&... args)
		{
			GENESIS_ENGINE_ASSERT(!this->has<T>(), "Entity already has component!");
			return this->scene->registry.emplace<T>(entity_handle, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& add_or_replace(Args&&... args)
		{
			return this->scene->registry.assign_or_replace<T>(entity_handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& get()
		{
			GENESIS_ENGINE_ASSERT(this->has<T>(), "Entity doesn't have component!");
			return this->scene->registry.get<T>(entity_handle);
		}

		template<typename T>
		bool has() const
		{
			return this->scene->registry.has<T>(entity_handle);
		}

		template<typename T>
		void remove()
		{
			GENESIS_ENGINE_ASSERT(this->has<T>(), "Entity doesn't have component!");
			this->scene->registry.remove<T>(entity_handle);
		}

		Scene* get_scene() const
		{
			return this->scene;
		}

		EntityHandle handle() const
		{
			return this->entity_handle;
		}

		bool valid() const
		{
			if (!this->scene)
				return false;

			return this->scene->registry.valid(this->entity_handle);
		}

		void destory()
		{
			this->scene->destoryEntity(*this);
			this->scene = nullptr;
			this->entity_handle = null_entity;
		}

		void addChild(Entity entity)
		{
			this->scene->addChild(*this, entity);
		}

		void removeChild(Entity entity)
		{
			this->scene->removeChild(*this, entity);
		}

		EntityHiearchy children()
		{
			return EntityHiearchy(&this->scene->registry, this->entity_handle);
		}

		bool operator==(const Entity& entity)
		{
			return (this->scene == entity.scene) && (this->entity_handle == entity.entity_handle);
		}

		bool operator!=(const Entity& entity)
		{
			return (this->scene != entity.scene) || (this->entity_handle != entity.entity_handle);
		}

		bool inSameScene(const Entity& entity)
		{
			return this->scene == entity.scene;
		}
	};
}