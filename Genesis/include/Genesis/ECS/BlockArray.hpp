#pragma once

#include "EntityTypes.hpp"

namespace Genesis
{
	/*
	* This is a class that will allocate some arbitrary number of fixed sized memory blocks
	*/
	class BlockArray
	{
	public:
		BlockArray(size_t block_size, size_t block_count)
			:block_size(block_size)
		{
			GENESIS_ENGINE_ASSERT((block_size > 0), "block_size must be greater than 0");

			this->block_count = block_count;
			if (this->block_count > 0)
			{
				this->array_data = new uint8_t[this->block_size * this->block_count];
			}
			else
			{
				this->array_data = nullptr;
			}
		};

		BlockArray()
			:block_size(0)
		{
			this->block_count = 0;
			this->array_data = nullptr;
		}

		~BlockArray()
		{
			if (this->array_data != nullptr)
			{
				delete[] this->array_data;
			}
		};

		size_t getBlockSize() { return this->block_size; };

		size_t getBlockCount() { return this->block_count; };

		void* getBlock(size_t index, size_t offset = 0)
		{
			GENESIS_ENGINE_ASSERT((index < this->block_count), "index must be less than block_count");
			GENESIS_ENGINE_ASSERT((offset < this->block_size), "offset must be less than block_size");
			return (void*)&array_data[(block_size * index) + offset];
		}

		void* data() { return (void*)array_data; };

		void resize(size_t new_block_count)
		{
			if (new_block_count == this->block_count || new_block_count == 0)
			{
				return;
			}

			uint8_t* new_data = new uint8_t[this->block_size * new_block_count];
			memcpy_s(new_data, this->block_size * new_block_count, this->array_data, this->block_size * this->block_count);
			delete[] this->array_data;
			this->array_data = new_data;
			this->block_count = new_block_count;
		}

		/*
		* Copy the data from one block to another
		*/
		void moveBlock(size_t dest_index, size_t source_index)
		{
			GENESIS_ENGINE_ASSERT((dest_index < this->block_count), "dest_index must be less than block_count");
			GENESIS_ENGINE_ASSERT((source_index < this->block_count), "source_index must be less than block_count");
			memcpy_s((void*)this->array_data[block_size * dest_index], this->block_size, (void*)this->array_data[block_size * source_index], this->block_size);
		}

	private:
		const size_t block_size;

		uint8_t* array_data;
		size_t block_count;
	};
};

