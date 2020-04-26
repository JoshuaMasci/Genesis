#pragma once

#include "Genesis/Core/Types.hpp"

namespace Genesis
{
	class FileSystem
	{
	public:
		static bool loadFileString(const string& filepath, string& destination);
		static bool loadFileBinary(const string& filepath, vector<uint8_t>& destination);
	};
}