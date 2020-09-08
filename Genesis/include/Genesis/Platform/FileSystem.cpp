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
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		ofn.lpstrDefExt = "";
		ofn.lpstrInitialDir = initial_directory.c_str();

		string fileNameStr;

		if (GetOpenFileName(&ofn))
		{
			fileNameStr = fileName;
		}

		return fileNameStr;
	}
#else
	string FileSystem::getFileDialog(const string& initial_directory)
	{
		return string();
	}
#endif

#include <string>

	bool FileSystem::loadShaderString(const string& filepath, string& destination)
	{
		size_t found = filepath.find_last_of("/\\");
		string path = filepath.substr(0, found);

		std::ifstream ShaderStream(filepath, std::ios::in);

		if (ShaderStream.is_open())
		{
			std::string Line = "";
			while (getline(ShaderStream, Line))
			{
				const string INCLUDE_DIRECTIVE = "#include \"";
				//If the line doesnt have an include statement
				//Add it to the Shader Code
				if (Line.find(INCLUDE_DIRECTIVE) == std::string::npos)
				{
					destination += "\n" + Line;
				}
				//If it has a include statement
				else
				{
					size_t includeLength = INCLUDE_DIRECTIVE.length();
					//SubString from the first " to the end "\n
					string include_file = path + "/" + Line.substr(includeLength, Line.length() - includeLength - 1) ;
					string include_file_data;
					
					GENESIS_ENGINE_ASSERT(loadShaderString(include_file, include_file_data), "Cannot Open Include");
					destination += include_file_data;
				}
			}
			ShaderStream.close();

			return true;
		}

		return false;
	}
}