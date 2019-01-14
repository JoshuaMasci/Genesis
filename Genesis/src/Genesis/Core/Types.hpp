#pragma once

//Vector Types
#include "Genesis/Core/VectorTypes.hpp"

#include <string>

#include "Genesis/Core/Array.hpp"

#include <vector>
#include <map>
#include <unordered_map>

namespace Genesis
{
	//Puts string in the Genesis namespace
	typedef std::string string;
	
	//STL replacements
	using Container::Array;

	//gives the ability to replace these later.
	using std::vector;
	using std::map;
	using std::unordered_map;
};

