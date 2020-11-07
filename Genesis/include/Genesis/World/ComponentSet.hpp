#pragma once

namespace Genesis
{
	class ComponentSet
	{
	protected:
		flat_hash_map<size_t, void*> components;

	public:

		~ComponentSet()
		{
			for (auto component_ptr : this->components)
			{
				delete component_ptr.second;
			}
		}

		template<typename T, typename... Args>
		T* addComponent(Args&&... args)
		{
			GENESIS_ENGINE_ASSERT(!this->hasComponent<T>(), "ComponentSet already has component");
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
			GENESIS_ENGINE_ASSERT(this->hasComponent<T>(), "ComponentSet doesn't have component");
			constexpr size_t hash_value = TypeInfo<T>::getHash();
			this->components[hash_value];
			this->components.erase(hash_value);
		}
	};
}