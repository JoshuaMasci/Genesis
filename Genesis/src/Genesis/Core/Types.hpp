#pragma once

//Vector Types
#include "Genesis/Core/VectorTypes.hpp"

#include <string>

//STL replacements
#include "Genesis/Core/Array.hpp"

#include <vector>
#include <set>
#include <map>
#include <unordered_map>

#define has_value(list, value) (list.find(value) != list.end())

namespace Genesis
{
	//Puts string in the Genesis namespace
	typedef std::string string;

	//gives the ability to replace these later.
	using std::vector;
	using std::set;
	using std::map;
	using std::unordered_map;
};

