#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"

#include "imgui.h"
#include "Genesis/Platform/FileSystem.hpp"

#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"

#include "Genesis/Resource/MeshPool.hpp"
#include "Genesis/Resource/MaterialPool.hpp"

namespace Genesis
{
	EntityPropertiesWindow::EntityPropertiesWindow(MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		this->mesh_pool = mesh_pool;
		this->material_pool = material_pool;
	}

	EntityPropertiesWindow::~EntityPropertiesWindow()
	{
	}

#define ADD_COMPONENT(entity, component, component_name, ...) if (!entity.hasComponent<component>() && ImGui::MenuItem(component_name)) { entity.addComponent<component>(__VA_ARGS__); }

	void EntityPropertiesWindow::draw(Entity* entity)
	{
		ImGui::Begin("Entity Properties");

		if (entity)
		{
			ImGui::InputText("Entity Name", entity->name.data, entity->name.SIZE);

			vector3D position = entity->local_transform.getPosition();
			if (ImGui::InputScalarN("Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
			{
				entity->local_transform.setPosition(position);
			};

			vector3D rotation = glm::degrees(glm::eulerAngles(entity->local_transform.getOrientation()));
			if (ImGui::InputScalarN("Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
			{
				entity->local_transform.setOrientation(quaternionD(glm::radians(rotation)));
			}

			vector3D scale = entity->local_transform.getScale();
			if (ImGui::InputScalarN("Scale", ImGuiDataType_::ImGuiDataType_Double, &scale, 3))
			{
				entity->local_transform.setScale(scale);
			}
		}

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