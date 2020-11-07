#pragma once

#include "Genesis/World/ComponentSet.hpp"
#include "Genesis/World/Hierarchy.hpp"

namespace Genesis
{
	struct WorldTest
	{
		ComponentSet components;
		Hierarchy hierarchy;
	};

	namespace Experimental
	{
		class Entity;

		class World
		{
		protected:
			Entity* entity_parent = nullptr;
			vector_set<Entity*> root_entities;

			//World Components
			flat_hash_map<size_t, void*> components;
		public:
			World();
			~World();

			void setEntityParent(Entity* entity);
			Entity* getEntityParent();

			void addEntity(Entity* entity);
			void removeEntity(Entity* entity);

			vector_set<Entity*>& getEntities() { return this->root_entities; };

			template<typename T, typename... Args>
			T* addComponent(Args&&... args)
			{
				GENESIS_ENGINE_ASSERT(!this->hasWorldComponent<T>(), "Entity already has component");
				constexpr size_t hash_value = TypeInfo<T>::getHash();
				this->components[hash_value] = (void*) new T(std::forward<Args>(args)...);
				return (T*)this->components[hash_value];
			}

			template<typename T>
			T* getComponent()
			{
				constexpr size_t hash_value = TypeInfo<T>::getHash();
				auto it = this->components.find(hash_value);
				if (it != this->components.end())
				{
					return (T*)this->components[hash_value];
				}
				else
				{
					return nullptr;
				}
			}

			template<typename T>
			bool hasComponent()
			{
				constexpr size_t hash_value = TypeInfo<T>::getHash();
				return this->components.find(hash_value) != this->components.end();
			}

			template<typename T>
			void removeComponent()
			{
				GENESIS_ENGINE_ASSERT(this->hasWorldComponent<T>(), "Entity doesn't have component");
				constexpr size_t hash_value = TypeInfo<T>::getHash();
				this->components[hash_value];
				this->components.erase(hash_value);
			}
		};
	}
}