#pragma once

#include "Genesis/Core/TypeInfo.hpp"

#include "Genesis/Component/NameComponent.hpp"

namespace Genesis
{
	namespace Experimental
	{
		class Entity
		{
		protected:
			//Base Info
			const size_t id;
			NameComponent name;
			TransformF local_transform;

			//Hierarchy
			Entity* parent = nullptr;
			vector_set<Entity*> children;

			//Components
			flat_hash_map<size_t, void*> components;
		public:
			Entity(size_t id, const char* name);
			~Entity();

			size_t getId();
			char* getName();
			TransformF& getLocalTransform();

			void addChild(Entity* entity);
			void removeChild(Entity* entity);
			vector_set<Entity*>& getChildren() { return this->children; };

			template<typename T, typename... Args>
			T* addComponent(Args&&... args)
			{
				GENESIS_ENGINE_ASSERT(!this->hasComponent<T>(), "Entity already has component");
				constexpr size_t hash_value = TypeInfo<T>::getHash();
				this->components[hash_value] = (void*) new T(std::forward<Args>(args)...);
				return (T*) this->components[hash_value];
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
				GENESIS_ENGINE_ASSERT(this->hasComponent<T>(), "Entity doesn't have component");
				constexpr size_t hash_value = TypeInfo<T>::getHash();
				this->components[hash_value];
				this->components.erase(hash_value);
			}
		};
	}
}