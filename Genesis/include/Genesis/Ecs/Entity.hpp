#pragma once

#include "Genesis/Ecs/Ecs.hpp"

#include "Genesis/Component/Hierarchy.hpp"

namespace Genesis
{
	class Entity
	{
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

		EntityRegistry* getRegistry()
		{
			return this->registry;
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

		void addChild(Entity entity)
		{
			Hierarchy::addChild(this->registry, this->entity_handle, entity.entity_handle);
		}

		void removeChild(Entity entity)
		{
			Hierarchy::removeChild(this->registry, this->entity_handle, entity.entity_handle);
		}

	protected:
		EntityHandle entity_handle = null_entity;
		EntityRegistry* registry = nullptr;
	};

	class EntityChildren
	{
	protected:
		EntityRegistry* registry;
		EntityHandle parent;

	public:
		EntityChildren(Entity parent) : parent(parent.getHandle()), registry(parent.getRegistry()) {};
		EntityChildren(EntityHandle parent, EntityRegistry* registry) : parent(parent), registry(registry) {};

		class Iterator
		{
		protected:
			EntityRegistry* registry;
			EntityHandle child;

		public:
			Iterator(EntityHandle child, EntityRegistry* registry) : child(child), registry(registry) {};

			Iterator& operator++()
			{
				if (this->registry->valid(this->child))
				{
					ChildNode* child_node = this->registry->try_get<ChildNode>(this->child);
					if (child_node != nullptr)
					{
						this->child = child_node->next;
					}
					else
					{
						this->child = null_entity;
					}
				}
				else
				{
					this->child = null_entity;
				}

				return *this;
			};

			Iterator operator++(int)
			{
				Iterator iterator = *this;

				if (iterator.registry->valid(iterator.child))
				{
					ChildNode* child_node = iterator.registry->try_get<ChildNode>(iterator.child);
					if (child_node != nullptr)
					{
						iterator.child = child_node->next;
					}
					else
					{
						iterator.child = null_entity;
					}
				}
				else
				{
					iterator.child = null_entity;
				}

				return iterator;
			};

			bool operator==(const Iterator& iterator)
			{
				return this->child == iterator.child;
			};

			bool operator!=(const Iterator& iterator)
			{
				return this->child != iterator.child;
			};

			Entity operator*()
			{
				return Entity(child, registry);
			};
		};

		Iterator begin()
		{
			if (this->registry->valid(this->parent))
			{
				ParentNode* parent_node = this->registry->try_get<ParentNode>(this->parent);
				if (parent_node != nullptr)
				{
					return Iterator(parent_node->first, this->registry);
				}
			}

			return Iterator(null_entity, this->registry);
		}

		Iterator end()
		{
			return Iterator(null_entity, this->registry);
		}
	};
}