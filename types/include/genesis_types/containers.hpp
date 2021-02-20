#pragma once

#include <parallel_hashmap/phmap.h>
#include <concurrentqueue.h>

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace genesis_types
{
	using phmap::flat_hash_map;
	using phmap::flat_hash_set;
	using moodycamel::ConcurrentQueue;

	using std::string;
	using std::vector;
	using std::function;

	using std::unique_ptr;
	using std::shared_ptr;
	using std::weak_ptr;
}