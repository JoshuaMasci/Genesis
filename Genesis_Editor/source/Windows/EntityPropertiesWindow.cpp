#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"

#include "imgui.h"
#include "Genesis/Platform/FileSystem.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"

#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"

#include "Genesis/Scene/Entity.hpp"

namespace Genesis
{
	template<class Component>
	void drawComponent(Entity& entity, const char* component_name, function<void(Component&)> draw_function)
	{
		if (entity.has<Component>())
		{
			bool header_open = ImGui::CollapsingHeader(component_name, ImGuiTreeNodeFlags_DefaultOpen);

			if (ImGui::BeginPopupContextItem(component_name, ImGuiMouseButton_Right))
			{
				if (ImGui::MenuItem("Delete Component"))
				{
					entity.remove<Component>();
					header_open = false;
				}
				ImGui::EndPopup();
			}

			if (header_open)
			{
				draw_function(entity.get<Component>());
			}
		}
	};

	template<class Component>
	void drawComponentLamda(Entity& entity, const char* component_name, void(*draw_function)(Component&))
	{
		drawComponent<Component>(entity, component_name, function<void(Component&)>(draw_function));
	}

#define ADD_COMPONENT(entity, component, component_name, ...) if (!entity.has<component>() && ImGui::MenuItem(component_name)) { entity.addComponent<component>(__VA_ARGS__); }

	void EntityPropertiesWindow::draw(Entity entity, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		ImGui::Begin("Entity Properties");

		if (!entity.valid())
		{
			ImGui::End();
			return;
		}

		drawComponent<NameComponent>(entity, "Name Component", [](NameComponent& name_component)
		{
			ImGui::InputText("Entity Name", name_component.data, name_component.SIZE);
		});

		drawComponent<Transform>(entity, "Transform", [](Transform& transform_component)
		{
			vector3D position = transform_component.getPosition();
			if (ImGui::InputScalarN("Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
			{
				transform_component.setPosition(position);
			};

			vector3D rotation = glm::degrees(glm::eulerAngles(transform_component.getOrientation()));
			if (ImGui::InputScalarN("Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
			{
				transform_component.setOrientation(quaternionD(glm::radians(rotation)));
			}

			vector3D scale = transform_component.getScale();
			if (ImGui::InputScalarN("Scale", ImGuiDataType_::ImGuiDataType_Double, &scale, 3))
			{
				transform_component.setScale(scale);
			}
		});

		drawComponent<ModelComponent>(entity, "Model Component", [=](ModelComponent& model_component)
		{
			const char* mesh_name = " ";
			if (model_component.mesh)
			{
				mesh_name = model_component.mesh->getName().c_str();
			}

			ImGui::LabelText("Mesh", mesh_name);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH"))
				{
					GENESIS_ENGINE_ASSERT(payload->DataSize > 0, "Payload Data Size wrong size");
					string file_path = string((char*)payload->Data);
					string extention = FileSystem::getExtention(file_path);

					if (extention == ".obj")
					{
						model_component.mesh = mesh_pool->getResource(file_path);
					}
				}
			}

			const char* material_name = " ";
			if (model_component.material)
			{
				material_name = model_component.material->getName().c_str();
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
						model_component.material = material_pool->getResource(file_path);
					}
				}
			}
		});


		ImGui::Separator();

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("add_component");
		}

		if (ImGui::BeginPopup("add_component"))
		{
			ImGui::EndPopup();
		}

		ImGui::End();
	}
}