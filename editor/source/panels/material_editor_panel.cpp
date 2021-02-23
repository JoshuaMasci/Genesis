#include "genesis_editor/panels/material_editor_panel.hpp"

#include "imgui.h"
#include "genesis_engine/resource/resource_manager.hpp"
#include "genesis_engine/platform/file_system.hpp"

namespace genesis
{
	MaterialEditorWindow::MaterialEditorWindow(ResourceManager* resource_manager)
	{
		this->resource_manager = resource_manager;
		this->grid_texture = this->resource_manager->texture_pool.getResource("res/textures/grid16.png");
	}

	void MaterialEditorWindow::setActiveMaterial(shared_ptr<Material> material)
	{
		this->active_material = material;
	}

	void MaterialEditorWindow::drawTexture(Material::MaterialTexture& texture_slot)
	{
		const ImVec2 IMAGE_SIZE(96.0f, 96.0f);

		const char* name = " ";
		if (texture_slot.texture)
		{
			name = texture_slot.texture->getName().c_str();
			ImGui::Image(texture_slot.texture->texture, IMAGE_SIZE);
			ImGui::OpenPopupOnItemClick("CLEAR_TEXTURE_POPUP", 1);
		}
		else
		{
			ImGui::Image(this->grid_texture->texture, IMAGE_SIZE);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize > 0, "Payload Data Size wrong size");
				string file_path = string((char*)payload->Data);
				string extention = FileSystem::getExtention(file_path);

				if (extention == ".png")
				{
					texture_slot.texture = this->resource_manager->texture_pool.getResource(file_path);
				}
			}
		}

		if (ImGui::BeginPopup("CLEAR_TEXTURE_POPUP"))
		{
			if (ImGui::MenuItem("Clear")) 
			{ 
				texture_slot.texture.reset();
				texture_slot.uv = -1;
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		ImGui::Text(name);

		if (!texture_slot.texture)
		{
			texture_slot.uv = -1;
		}

		const char* UV_COORD_NAME[] = { "OFF", "UV1", "UV2" };
		if (ImGui::BeginCombo("uv coords", UV_COORD_NAME[texture_slot.uv + 1]))
		{
			for (int i = 0; i < _countof(UV_COORD_NAME); i++)
			{
				if (ImGui::Selectable(UV_COORD_NAME[i], texture_slot.uv == (i - 1)))
				{
					texture_slot.uv = i - 1;
				}
			}
			ImGui::EndCombo();
		}
	}

	void MaterialEditorWindow::draw()
	{
		ImGui::Begin("Material");

		const char* material_name = " ";

		if (this->active_material)
		{
			material_name = this->active_material->getName().c_str();
		}

		ImGui::LabelText("Material", material_name);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH"))
			{
				GENESIS_ENGINE_ASSERT(payload->DataSize > 0, "Payload Data Size wrong size");
				string file_path = string((char*)payload->Data);
				string extention = FileSystem::getExtention(file_path);

				if (extention == ".mat")
				{
					this->active_material = this->resource_manager->material_pool.getResource(file_path);
				}
			}
		}

		if (this->active_material)
		{
			{
				ImGui::PushID("Albedo");

				if (ImGui::CollapsingHeader("Albedo", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
				{
					drawTexture(this->active_material->albedo_texture);
					ImGui::ColorEdit4("Color", &this->active_material->albedo_factor.x);
				}
				ImGui::PopID();
			}

			{
				ImGui::PushID("Normal");
				if (ImGui::CollapsingHeader("Normal", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
				{
					drawTexture(this->active_material->normal_texture);
				}
				ImGui::PopID();
			}

			{
				ImGui::PushID("MetallicRoughness");
				if (ImGui::CollapsingHeader("Metallic Roughness", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
				{
					drawTexture(this->active_material->metallic_roughness_texture);
					if (ImGui::DragFloat("Metallic", &this->active_material->metallic_roughness_factor.x, 0.005f, 0.0f, 1.0f))
					{
						this->active_material->metallic_roughness_factor.x = std::clamp(this->active_material->metallic_roughness_factor.x, 0.0f, 1.0f);
					}
					if (ImGui::DragFloat("Roughness", &this->active_material->metallic_roughness_factor.y, 0.005f, 0.0f, 1.0f))
					{
						this->active_material->metallic_roughness_factor.y = std::clamp(this->active_material->metallic_roughness_factor.y, 0.0f, 1.0f);
					}
				}
				ImGui::PopID();
			}

			{
				ImGui::PushID("Occlusion");
				if (ImGui::CollapsingHeader("Occlusion", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
				{
					drawTexture(this->active_material->occlusion_texture);
				}
				ImGui::PopID();
			}

			{
				ImGui::PushID("Emissive");
				if (ImGui::CollapsingHeader("Emissive", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
				{
					drawTexture(this->active_material->emissive_texture);
					ImGui::ColorEdit4("Color", &this->active_material->emissive_factor.x);
				}
				ImGui::PopID();
			}

			{
				ImGui::PushID("Settings");
				if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox("Transparent", &this->active_material->transparent);
					ImGui::Checkbox("Cull Backface", &this->active_material->cull_backface);
				}
				ImGui::PopID();
			}
		}

		ImGui::End();
	}
}