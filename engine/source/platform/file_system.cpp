#include "genesis_engine/platform/file_system.hpp"

#include <fstream>
#include <string>
#include <filesystem>

namespace genesis 
{
#ifdef GENESIS_PLATFORM_WIN
#include <windows.h>
#include <commdlg.h> 
	string FileSystem::openFileDialog(const char* filter)
	{
		OPENFILENAME open_file_name;
		char file_path[MAX_PATH] = "";
		ZeroMemory(&open_file_name, sizeof(open_file_name));

		open_file_name.lStructSize = sizeof(OPENFILENAME);
		open_file_name.hwndOwner = NULL;
		open_file_name.lpstrFilter = filter;
		open_file_name.lpstrFile = file_path;
		open_file_name.nMaxFile = MAX_PATH;
		open_file_name.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
		open_file_name.lpstrDefExt = "";
		open_file_name.lpstrInitialDir = "";

		string file_name_str;

		if (GetOpenFileName(&open_file_name))
		{
			file_name_str = file_path;
			std::replace(file_name_str.begin(), file_name_str.end(), '\\', '/');
		}

		return file_name_str;
	}

	string FileSystem::saveFileDialog(const char* filter)
	{
		OPENFILENAME open_file_name;
		char file_path[MAX_PATH] = "";
		ZeroMemory(&open_file_name, sizeof(open_file_name));

		open_file_name.lStructSize = sizeof(OPENFILENAME);
		open_file_name.hwndOwner = NULL;
		open_file_name.lpstrFilter = filter;
		open_file_name.lpstrFile = file_path;
		open_file_name.nMaxFile = MAX_PATH;
		open_file_name.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		open_file_name.lpstrDefExt = "";
		open_file_name.lpstrInitialDir = "";

		string file_name_str;

		if (GetSaveFileName(&open_file_name))
		{
			file_name_str = file_path;
			std::replace(file_name_str.begin(), file_name_str.end(), '\\', '/');
		}

		return file_name_str;
	}

#else
	string FileSystem::openFileDialog(const char* filter)
	{
		return string();
	}

	string FileSystem::saveFileDialog(const char* filter)
	{
		return string();
	}
#endif


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

	void FileSystem::readDirectory(const string& directory_path, vector<string>& files)
	{
		string filepath_temp = directory_path;
		std::replace(filepath_temp.begin(), filepath_temp.end(), '/', '\\');

		std::filesystem::path path(filepath_temp);
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
		string filepath_temp = directory_path;
		std::replace(filepath_temp.begin(), filepath_temp.end(), '/', '\\');

		std::filesystem::path path(filepath_temp);
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