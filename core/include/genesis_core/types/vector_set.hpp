#pragma once

#include <vector>

namespace genesis
{
	template<typename T>
	class VectorSet
	{
	protected:
		std::vector<T> vector;
	public:
		using iterator = typename std::vector<T>::reverse_iterator;
		using const_iterator = typename std::vector<T>::const_reverse_iterator;

		void insert(const T& type)
		{
			this->vector.push_back(type);
		};

		void erase(const T& type)
		{
			for (size_t i = 0; i < this->vector.size(); i++)
			{
				if (this->vector[i] == type)
				{
					//is last element
					const size_t last_element = this->vector.size() - 1;
					if (i != last_element)
					{
						this->vector[i] = this->vector[last_element];
					}

					this->vector.pop_back();
					return;
				}
			}
		};

		bool has(const T& type)
		{
			for (T value : this->vector)
			{
				if (value == type)
				{
					return true;
				}
			}
			return false;
		};

		iterator begin()
		{
			return this->vector.rbegin();
		}

		iterator end()
		{
			return this->vector.rend();
		}

		const_iterator cbegin()
		{
			return this->vector.crbegin();
		}

		const_iterator cend()
		{
			return this->vector.crend();
		}

		bool empty()
		{
			return this->vector.empty();
		}

		std::vector<T> copyVector()
		{
			return this->vector;
		}
	};
}