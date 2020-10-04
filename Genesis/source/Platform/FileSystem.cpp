#include "Genesis/Platform/FileSystem.hpp"

#include <fstream>
#include <string>
#include <filesystem>

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

//TODO replace with IMGUI version
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

	struct path_leaf_string
	{
		std::string operator()(const std::filesystem::directory_entry& entry) const
		{
			return entry.path().filename().string();
		}
	};

	void FileSystem::readDirectory(const string& filepath, vector<string>& files)
	{
		std::filesystem::path path(filepath);
		std::filesystem::directory_iterator start(path);
		std::filesystem::directory_iterator end;
		std::transform(start, end, std::back_inserter(files), path_leaf_string());
	}

	struct path_info
	{
		FileInfo operator()(const std::filesystem::directory_entry& entry) const
		{
			std::filesystem::path path = entry.path();
			string relative_path = path.relative_path().string();
			std::replace(relative_path.begin(), relative_path.end(), '\\', '/');
			return { relative_path, path.filename().string(), path.extension().string(), entry.is_directory() };
		}
	};

	void FileSystem::readDirectory(const string& directory_path, vector<FileInfo>& files)
	{
		std::filesystem::path path(directory_path);
		std::filesystem::directory_iterator start(path);
		std::filesystem::directory_iterator end;
		std::transform(start, end, std::back_inserter(files), path_info());
	}
	
	string FileSystem::getPath(const string& filepath)
	{
		auto index = filepath.find_last_of("/");
		if (index == std::string::npos)
		{
			return filepath;
		}
		return filepath.substr(0, index + 1);
	}

	string FileSystem::getFilename(const string& filepath)
	{
		auto index = filepath.find_last_of("/");
		if (index == std::string::npos)
		{
			return filepath;
		}
		return filepath.substr(index + 1);
	}

	string FileSystem::getExtention(const string& filepath)
	{
		auto index = filepath.find_last_of('.');
		if (index == std::string::npos)
		{
			return "";
		}

		return filepath.substr(index);
	}
}