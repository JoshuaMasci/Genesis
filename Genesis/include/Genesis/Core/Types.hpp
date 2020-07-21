#pragma once

#include <parallel_hashmap/phmap.h>
#include <concurrentqueue.h>

#include <string>
#include <vector>
#include <bitset>
#include <functional>

#define has_value(list, value) (list.find(value) != list.end())

namespace Genesis
{
	typedef double TimeStep;

	//Puts these classes in the Genesis namespace
	using phmap::flat_hash_map;
	using phmap::flat_hash_set;
	using moodycamel::ConcurrentQueue;

	using std::string;
	using std::vector;
	using std::bitset;
	using std::function;
	using std::hash;
};

