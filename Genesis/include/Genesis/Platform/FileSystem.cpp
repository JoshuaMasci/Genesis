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

#ifdef GENESIS_PLATFORM_WIN
#include <windows.h>
#include <commdlg.h> 
	string FileSystem::getFileDialog(const string& initial_directory)
	{
		OPENFILENAME ofn;
		char fileName[MAX_PATH] = "";
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = NULL;
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = "";

		string fileNameStr;

		if (GetOpenFileName(&ofn))
			fileNameStr = fileName;

		return fileNameStr;
	}
#else
	string FileSystem::getFileDialog(const string& initial_directory)
	{
		return string();
	}
#endif // GENESIS_PLATFORM_WIN
}