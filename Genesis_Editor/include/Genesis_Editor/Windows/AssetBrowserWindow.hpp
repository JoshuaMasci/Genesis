#pragma once

#include <unordered_map>
#include "Genesis/Platform/FileSystem.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class AssetBrowserWindow
	{
	private:
		LegacyBackend* backend = nullptr;

		string active_directory;
		std::unordered_map<string, vector<FileInfo>> directory_map;
		Texture2D test_texture = nullptr;
		

		void refresh(const string& project_directory);
		void readDirectory(const string& directory_path);

		void drawDirectoryTree(const string& directory_path);
		void drawDirectoryPanel(const string& directory_path);

	public:
		AssetBrowserWindow(LegacyBackend* backend);
		~AssetBrowserWindow();

		void draw(const string& project_directory);
	};
}