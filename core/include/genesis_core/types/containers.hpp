#pragma once

#include <parallel_hashmap/phmap.h>
#include <concurrentqueue.h>

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <memory>

#include "vector_set.hpp"

namespace genesis
{
	using phmap::flat_hash_map;
	using phmap::flat_hash_set;
	using moodycamel::ConcurrentQueue;

	using std::string;
	using std::vector;
	using std::optional;
	using std::function;

	using std::unique_ptr;
	using std::shared_ptr;
	using std::weak_ptr;

	template<typename T, typename ... Args>
	constexpr shared_ptr<T> make_shared(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	};
}