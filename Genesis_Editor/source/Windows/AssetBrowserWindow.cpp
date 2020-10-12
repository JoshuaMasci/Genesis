#include "Genesis_Editor/Windows/AssetBrowserWindow.hpp"

#include "imgui.h"

namespace Genesis
{
	void AssetBrowserWindow::refresh(const string& project_directory)
	{
		this->directory_map.clear();
		this->readDirectory(project_directory);
		this->active_directory = project_directory;
	}

	void AssetBrowserWindow::readDirectory(const string& directory_path)
	{
		vector<FileInfo>& map = this->directory_map[directory_path];
		FileSystem::readDirectory(directory_path, map);

		for (FileInfo info : map)
		{
			if (info.is_directory)
			{
				this->readDirectory(info.path);
			}
		}
	}

	void AssetBrowserWindow::drawDirectoryTree(const string& directory_path)
	{
		if (has_value(this->directory_map, directory_path))
		{
			vector<FileInfo>& map = this->directory_map[directory_path];

			for (FileInfo info : map)
			{
				if (info.is_directory)
				{
					const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow;
					ImGuiTreeNodeFlags node_flags = base_flags;

					if (info.path == this->active_directory)
					{
						node_flags |= ImGuiTreeNodeFlags_Selected;
					}

					if (has_value(this->directory_map, info.path))
					{
						auto& directory_contents = this->directory_map[info.path];

						bool has_sub_directory = false;

						for (auto& file : directory_contents)
						{
							if (file.is_directory)
							{
								has_sub_directory = true;
								break;
							}
						}

						if (!has_sub_directory)
						{
							node_flags |= ImGuiTreeNodeFlags_Leaf;
						}
					}
					else
					{
						node_flags |= ImGuiTreeNodeFlags_Leaf;
					}

					bool node_open = ImGui::TreeNodeEx(info.filename.c_str(), node_flags, info.filename.c_str());

					if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
					{
						this->active_directory = info.path;
					}

					if(node_open)
					{
						this->drawDirectoryTree(info.path);
						ImGui::TreePop();
					}
				}
			}
		}
	}

	void AssetBrowserWindow::drawDirectoryPanel(const string& directory_path)
	{
		if (has_value(this->directory_map, directory_path))
		{
			vector<FileInfo>& directory = this->directory_map[directory_path];

			const float item_size = 96.0f;

			float contentWidth = ImGui::GetContentRegionAvailWidth();

			int columns = (int)(contentWidth / item_size);
			columns = std::max(columns, 1);
			ImGui::Columns(columns, nullptr, false);

			//TODO add a back arrow directory button

			for (int i = 0; i < directory.size(); i++)
			{
				ImGui::PushID(i);
				ImGui::BeginGroup();

				Texture2D texture = this->file_icon;

				//TODO more file icons
				if (directory[i].is_directory)
				{
					texture = this->directory_icon;
				}

				ImGui::Image(texture, { item_size - 5, item_size - 5 });

				if (directory[i].is_directory)
				{
					if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) // change active directory
					{
						this->active_directory = directory[i].path;
					}
				}

				ImGui::TextWrapped(directory[i].filename.c_str());

				ImGui::EndGroup();
				ImGui::PopID();

				if (!directory[i].is_directory)
				{
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						const char* file_path = directory[i].path.c_str();
						size_t file_path_size = sizeof(char) * (directory[i].path.size() + 1); 
						ImGui::SetDragDropPayload("ASSET_FILE_PATH", file_path, file_path_size);
						ImGui::Text(directory[i].filename.c_str());
						ImGui::EndDragDropSource();
					}
				}

				ImGui::NextColumn();
			}
		}

	}

	AssetBrowserWindow::AssetBrowserWindow(LegacyBackend* backend)
	{
		this->backend = backend;

		const string starting_directory = "res/";
		this->refresh(starting_directory);

		//Using 1x1 textures for now
		//TODO replace with icons

		TextureCreateInfo create_info = {};
		create_info.size = vector2U(1, 1);
		create_info.format = ImageFormat::RGB_8;

		{
			uint8_t data[] = { 255, 5, 10 };
			this->file_icon = this->backend->createTexture(create_info, data);
		}

		{
			uint8_t data[] = { 5, 10, 255 };
			this->directory_icon = this->backend->createTexture(create_info, data);
		}
	}

	AssetBrowserWindow::~AssetBrowserWindow()
	{
		this->backend->destoryTexture(this->file_icon);
		this->backend->destoryTexture(this->directory_icon);
	}

	void AssetBrowserWindow::draw(const string& project_directory)
	{
		//ImGui::ShowDemoWindow();

		ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Refresh"))
				{
					const string starting_directory = "res/";
					this->refresh(starting_directory);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

		{
			const float MAX_TREE_CHILD_WIDTH = 250.0f;
			ImGui::BeginChild("Project_Tree", ImVec2(std::min(ImGui::GetWindowContentRegionWidth() * 0.25f, MAX_TREE_CHILD_WIDTH), 0.0f), true, 0);

			const ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;
			ImGuiTreeNodeFlags node_flags = base_flags;

			if (project_directory == this->active_directory)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool node_open = ImGui::TreeNodeEx("Project", node_flags);

			if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
			{
				this->active_directory = project_directory;
			}

			if (node_open)
			{
				this->drawDirectoryTree("res/");
				ImGui::TreePop();
			}

			ImGui::EndChild();
		}

		ImGui::SameLine();

		{
			ImGui::BeginChild("Asset_List", ImVec2(0.0f, 0.0f), true, 0);

			this->drawDirectoryPanel(this->active_directory);

			ImGui::EndChild();
		}

		ImGui::PopStyleVar();

		ImGui::End();
	}
}