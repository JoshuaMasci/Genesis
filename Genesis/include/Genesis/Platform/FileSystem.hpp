#pragma once

namespace Genesis
{
	struct FileInfo
	{
		string path;
		string filename;
		string extention;
		bool is_directory;
	};

	class FileSystem
	{
	public:
		static bool loadFileString(const string& filepath, string& destination);
		static bool loadFileBinary(const string& filepath, vector<uint8_t>& destination);

		static string getFileDialog(const string& initial_directory);

		static bool loadShaderString(const string& filepath, string& destination);

		static void readDirectory(const string& filepath, vector<string>& files);
		static void readDirectory(const string& directory_path, vector<FileInfo>& files);

		static string getPath(const string& filepath);
		static string getFilename(const string& filepath);
		static string getExtention(const string& filepath);
	};
}