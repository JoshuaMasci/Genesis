#pragma once

#include "Genesis/Debug/Assert.hpp"

namespace Genesis
{
	class MemoryArray
	{
	public:
		MemoryArray(size_t block_size, size_t block_count = 10)
		{
			GENESIS_ENGINE_ASSERT_ERROR((block_size > 0), "block_size must be greater than 0");
			GENESIS_ENGINE_ASSERT_ERROR((block_count > 0), "block_count must be greater than 0");

			this->block_size = block_size;
			this->block_count = block_count;
			this->array_data = new uint8_t[this->block_size * this->block_count];
		};

		~MemoryArray()
		{
			if (this->array_data != nullptr)
			{
				delete[] this->array_data;
			}
		};

		size_t getBlockCount() { return this->block_count; };

		void* getBlock(size_t index, size_t offset)
		{
			GENESIS_ENGINE_ASSERT_ERROR((index < this->block_count), "index must be less than block_count");
			GENESIS_ENGINE_ASSERT_ERROR((offset < this->block_size), "offset must be less than block_size");
			return (void*)&array_data[(block_size * index) + offset];
		}

		void* data() { return (void*)array_data; };

		void increaseSize(size_t count)
		{
			if (count == 0)
			{
				return;
			}

			size_t new_count = this->block_count + count;
			uint8_t* new_data = new uint8_t[this->block_size * new_count];

			memcpy_s(new_data, this->block_size * new_count, this->array_data, this->block_size * this->block_count);
			delete[] this->array_data;
			this->array_data = new_data;
			this->block_count = new_count;
		};

		void moveBlock(size_t dest_index, size_t source_index)
		{
			GENESIS_ENGINE_ASSERT_ERROR((dest_index < this->block_count), "dest_index must be less than block_count");
			GENESIS_ENGINE_ASSERT_ERROR((source_index < this->block_count), "source_index must be less than block_count");
			memcpy_s((void*)this->array_data[block_size * dest_index], this->block_size, (void*)this->array_data[block_size * source_index], this->block_size);
		}

	private:
		size_t block_size;

		uint8_t* array_data;
		size_t block_count;
	};
};

