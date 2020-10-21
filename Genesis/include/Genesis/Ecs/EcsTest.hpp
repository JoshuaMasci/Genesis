#pragma once

namespace Genesis
{
	class BlockArray
	{
	protected:
		const size_t block_size;
		size_t block_capacity;
		size_t block_next;
		uint8_t* data;

	public:
		BlockArray(size_t block_size, size_t starting_capacity = 10)
			:block_size(block_size)
		{
			GENESIS_ENGINE_ASSERT(block_size > 0, "block_size must be greater than 0");
			this->block_capacity = starting_capacity;
			this->block_next = 0;
			this->data = new uint8_t[block_size * this->block_capacity];
		}

		~BlockArray()
		{
			delete this->data;
		}

		void* operator[](size_t block_index)
		{
			GENESIS_ENGINE_ASSERT(block_index >= this->block_next, "block_index out of range");
			return (void*)&this->data[this->block_size * block_index];
		}

		size_t push_back()
		{
			//Increase Capacity
			if (this->block_next == this->block_capacity)
			{
				size_t new_capacity = this->block_capacity * 2;
				uint8_t* new_data = new uint8_t[this->block_size * new_capacity];
				memcpy_s(new_data, this->block_size * new_capacity, this->data, this->block_size * this->block_capacity);
				delete[] this->data;
				this->data = new_data;
				this->block_capacity = new_capacity;
			}

			size_t block_index = this->block_next;
			this->block_next++;
			return block_index;
		}

		void pop_back()
		{
			this->block_next--;
		}
	};

	namespace Experimental
	{
		template <typename T>
		struct TypeInfo
		{
			static size_t getHash()
			{
				return typeid(T).hash_code();
			}
		};

		typedef unsigned short EntityHandle;

		template <typename T>
		class ComponentPool
		{
		protected:
			BlockArray data;

		public:
			ComponentPool()
			{

			};

			~ComponentPool()
			{

			};

		};

		class Registry
		{
		protected:
			vector<EntityHandle> freed_handles;
			EntityHandle next_handle = 0;

			EntityHandle getNextId()
			{
				EntityHandle entity_handle = this->next_handle;

				if (freed_handles.empty())
				{
					this->next_handle++;
				}
				else
				{
					entity_handle = this->freed_handles[this->freed_handles.size()];
					this->freed_handles.pop_back();
				}
				return entity_handle;
			};

		public:

			EntityHandle create()
			{
				return 0;
			};
			
			void destory(const EntityHandle entity)
			{

			};
			
			bool valid(const EntityHandle entity)
			{
				return false;
			};

			template<typename Component, typename... Args>
			Component& add(const EntityHandle entity, Args &&... args)
			{

			};

			template<typename Component>
			void remove(const EntityHandle entity)
			{

			};

			template<typename Component>
			bool has(const EntityHandle entity)
			{

			};

			template<typename Component>
			Component& get(const EntityHandle entity)
			{

			};

			template<typename Component>
			Component* try_get(const EntityHandle entity)
			{

			};
		};
	}
}