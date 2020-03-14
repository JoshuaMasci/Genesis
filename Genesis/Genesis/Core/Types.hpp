#pragma once

#include <string>

//STL replacements
#include "Genesis/Core/List.hpp"

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <queue>

#define has_value(list, value) (list.find(value) != list.end())

namespace Genesis
{
	typedef double TimeStep;

	//Puts string in the Genesis namespace
	typedef std::string string;

	//gives the ability to replace these later.
	using std::vector;
	using std::set;
	using std::map;
	using std::unordered_map;
	using std::queue;
};
