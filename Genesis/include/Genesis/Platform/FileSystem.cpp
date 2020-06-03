#include "Genesis/Platform/FileSystem.hpp"

#include <fstream>

namespace Genesis 
{
	bool FileSystem::loadFileString(const string& filepath, string& destination)
	{
		std::ifstream file_reader(filepath, std::ios::ate);
		if (file_reader.is_open())
		{
			size_t fileSize = (size_t)file_reader.tellg();
			file_reader.seekg(0);
			destination.resize(fileSize);
			destination.assign((std::istreambuf_iterator<char>(file_reader)), (std::istreambuf_iterator<char>()));
			file_reader.close();
			return true;
		}
		return false;
	}

	bool FileSystem::loadFileBinary(const string& filepath, vector<uint8_t>& destination)
	{
		std::ifstream file_reader(filepath, std::ios::ate || std::ios::binary);
		if (file_reader.is_open())
		{
			size_t fileSize = (size_t)file_reader.tellg();
			file_reader.seekg(0);
			destination.resize(fileSize);
			destination.assign((std::istreambuf_iterator<char>(file_reader)), (std::istreambuf_iterator<char>()));
			file_reader.close();

			return true;
		}
		return false;
	}
}