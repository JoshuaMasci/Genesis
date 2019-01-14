#pragma once

namespace Container
{
	template<typename T>
	class Array
	{
	public:
		Array(){};//Empty Array
		Array(size_t size)
		{
			this->array_size = size;
			this->data = new T[this->array_size];
		};

		~Array()
		{
			if (this->data != nullptr)
			{
				delete this->data;
			}
		};

		size_t size() { return this->array_size; };

		T& operator[](size_t index)
		{
			if (index >= this->array_size)
			{
				return nullptr; //TODO crash
			}

			return this->data[index];
		}

	private:
		size_t array_size = 0;
		T* data = nullptr;
	};
};

