#pragma once

namespace Genesis
{
	class FileSystem
	{
	public:
		static bool loadFileString(const string& filepath, string& destination);
		static bool loadFileBinary(const string& filepath, vector<uint8_t>& destination);

		static string getFileDialog(const string& initial_directory);

		static bool loadShaderString(const string& filepath, string& destination);
	};
}