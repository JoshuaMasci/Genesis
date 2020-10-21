#pragma once

namespace Genesis
{
	template<typename Key, typename Component>
	class ComponentStorage
	{
	protected:
		vector<Key> key_array;
		vector<Component> component_array;
		flat_hash_map<Key, size_t> key_map;

	public:
		Component& add(Key key)
		{
			GENESIS_ENGINE_ASSERT(!this->has(key), "Key already exists in table");
			size_t new_index = this->component_array.size();
			this->component_array.push_back(Component());
			this->key_array.push_back(key);
			this->key_map.insert({ key, new_index });
			return this->component_array[new_index];
		};

		void remove(Key key)
		{
			GENESIS_ENGINE_ASSERT(this->has(key), "Key doesn't exists in table");
			size_t key_index = this->key_map[key];
			size_t last_index = this->component_array.size() - 1;

			//Move last component into the removed slot
			if (key_index != last_index)
			{
				this->component_array[key_index] = this->component_array[last_index];
				this->key_array[key_index] = this->key_array[last_index];
				this->key_map[this->key_array[last_index]] = key_index;
			}

			this->component_array.pop_back();
			this->key_array.pop_back();
			this->key_map.erase(key);
		};

		bool has(Key key)
		{
			return this->key_map.find(key) != this->key_map.end();
		};

		Component& get(Key key)
		{
			GENESIS_ENGINE_ASSERT(this->has(key), "Key doesn't exists in table");
			return this->component_array[this->key_map[key]];
		};

		Component* try_get(Key key)
		{
			auto it = this->key_map.find(key);

			if (it != this->key_map.end())
			{
				return &this->component_array[it->second];
			}

			return nullptr;
		};

		//TODO Interator (reverse)
	};
}