#pragma once

#include <vector>

namespace Container
{
	template<typename T>
	class ObjectPool
	{
	public:
		ObjectPool(size_t size = 0)
		{
			vector.resize(size);
		};

		~ObjectPool()
		{

		};

		size_t size() { return 0; };
	private:
		std::vector<T> vector;
	};
}