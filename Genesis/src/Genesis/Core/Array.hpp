#pragma once

namespace Genesis
{
	template<typename T>
	class Array
	{
	public:
		Array(){};//Empty Array
		Array(size_t size)
		{
			if (size > 0)
			{
				this->array_size = size;
				this->array_data = new T[this->array_size];
			}
			else
			{
				this->array_size = 0;
				this->array_data = nullptr;
			}
		};

		~Array()
		{
			if (this->array_data != nullptr)
			{
				delete this->array_data;
				this->array_data = nullptr;
			}
		};

		size_t size() { return this->array_size; };

		T* data() { return array_data; };

		T& operator[](size_t index)
		{
			if (index >= this->array_size)
			{
				throw std::out_of_range("Genesis Array");
			}
			return this->array_data[index];
		};

		Array<T> &operator=(const Array<T> &right)
		{
			if (&right != this)
			{
				delete this->array_data;

				this->array_size = right.array_size;
				this->array_data = new T[this->array_size];

				for (int i = 0; i < this->array_size; i++)
				{
					this->array_data[i] = right.array_data[i];
				}
			}

			return *this;
		};

	private:
		size_t array_size = 0;
		T* array_data = nullptr;
	};
};

