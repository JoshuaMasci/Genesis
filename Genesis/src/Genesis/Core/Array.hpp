#pragma once

namespace Genesis
{
	template<typename T>
	class Array
	{
	public:
		Array(size_t size = 0)
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

		//Copy Constructor
		Array(const Array<T>& source)
		{
			this->array_size = source.array_size;
			this->array_data = new T[this->array_size];

			for (int i = 0; i < this->array_size; i++)
			{
				this->array_data[i] = source->array_data[i];
			}
		}

		~Array()
		{
			if (this->array_data != nullptr)
			{
				delete this->array_data;
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

		void resize(size_t new_size)
		{
			if (new_size == 0)
			{
				delete this->array_data;
				this->array_data = nullptr;
				this->array_size = 0;
				return;
			}
	
			T* new_array = new T[new_size];

			if (new_size >= this->array_size)
			{
				for (int i = 0; i < this->array_size; i++)
				{
					new_array[i] = this->array_data[i];
				}
			}
			else
			{
				for (int i = 0; i < new_size; i++)
				{
					new_array[i] = this->array_data[i];
				}
			}

			delete this->array_data;
			this->array_data = new_array;
			this->array_size = new_size;
		};

	private:
		size_t array_size = 0;
		T* array_data = nullptr;
	};
};

