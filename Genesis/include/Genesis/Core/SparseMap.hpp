#pragma once

namespace Genesis
{
	template<typename KeyType, typename Type>
	class SparseMap
	{
	protected:
		struct DenseType
		{
			KeyType key = 0;
			Type data;
		};

		vector<DenseType> dense_array;
		vector<size_t> sparse_array;

		const size_t INVALID_INDEX = std::numeric_limits<size_t>::max()

	public:
		SparseMap()
		{

		};

		~SparseMap()
		{

		};

		bool has(KeyType key)
		{
			if (key < this->sparse_array.size())
			{
				return this->sparse_array[key] != INVALID_INDEX;
			}

			return false;
		}

		Type& get(KeyType key)
		{
			return this->dense_array[this->sparse_array[key]].data;
		};

		Type* try_get(KeyType key)
		{
			if (this->has(key))
			{
				return &this->dense_array[this->sparse_array[key]].data;
			}
			else
			{
				return nullptr;
			}
		}

		Type& add(KeyType key)
		{
			GENESIS_ENGINE_ASSERT(!this->has(key), "SparseMap already has key");
			size_t new_index = this->dense_array.size();

		}
	};
}