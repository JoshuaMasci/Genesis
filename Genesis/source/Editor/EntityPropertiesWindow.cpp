#include "Genesis/Editor/EntityPropertiesWindow.hpp"

#include "Genesis/Entity/Entity.hpp"
#include "Genesis/Entity/NameComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Physics/RigidBody.hpp"

using namespace Genesis;

EntityPropertiesWindow::EntityPropertiesWindow()
{
}

EntityPropertiesWindow::~EntityPropertiesWindow()
{
}

#include "imgui.h"


void EntityPropertiesWindow::drawWindow(World* world, Entity* selected_entity)
{
	ImGui::ShowDemoWindow();

	ImGui::Begin("Entity Properties");

	if (selected_entity != nullptr)
	{
		if(ImGui::CollapsingHeader("Entity Properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const size_t entity_name_size = 128;
			char entity_name[entity_name_size];
			strcpy_s(entity_name, entity_name_size, selected_entity->getName().c_str());
			if (ImGui::InputText("Entity Name", entity_name, entity_name_size))
			{
				selected_entity->setName(string(entity_name));
			}


			{
				bool has_changed_transform = false;
				TransformD local_transform = selected_entity->getLocalTransform();

				vector3D position = local_transform.getPosition();
				if (ImGui::InputScalarN("Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
				{
					local_transform.setPosition(position);
					has_changed_transform = true;
				};

				vector3D rotation = glm::degrees(glm::eulerAngles(local_transform.getOrientation()));
				if (ImGui::InputScalarN("Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
				{
					local_transform.setOrientation(quaternionD(glm::radians(rotation)));
					has_changed_transform = true;
				}

				vector3D scale = local_transform.getScale();
				if (ImGui::InputScalarN("Scale", ImGuiDataType_::ImGuiDataType_Double, &scale, 3))
				{
					local_transform.setScale(scale);
					has_changed_transform = true;
				};

				if (has_changed_transform)
				{
					selected_entity->setLocalTransform(local_transform);
				}
			}

		}

	}
	else
	{
		ImGui::TextUnformatted("Error: Invalid Entity");
	}

	ImGui::End();
}

/*if (registry->has<NameComponent>(entity))
		{
			NameComponent& name_component = registry->get<NameComponent>(entity);

			if (ImGui::CollapsingHeader("NameComponent", ImGuiTreeNodeFlags_None))
			{
				const size_t entity_name_size = 128;
				char entity_name[entity_name_size];
				strcpy_s(entity_name, entity_name_size, name_component.name.c_str());

				if (ImGui::InputText("Entity Name", entity_name, entity_name_size))
				{
					name_component.name = string(entity_name);
				}
			}
		}


		if (registry->has<TransformD>(entity))
		{
			TransformD& entity_transform = registry->get<TransformD>(entity);

			if (ImGui::CollapsingHeader("TransformD", ImGuiTreeNodeFlags_None))
			{
				{
					vector3D position = entity_transform.getPosition();
					if (ImGui::InputScalarN("Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
					{
						entity_transform.setPosition(position);
					};
				}
				{
					//TODO rotation
					vector3D rotation = glm::degrees(glm::eulerAngles(entity_transform.getOrientation()));
					ImGui::InputScalarN("Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3);
					//entity_transform.setOrientation(quaternionD(glm::radians(rotation)));
				}
				{
					vector3D scale = entity_transform.getScale();
					if (ImGui::InputScalarN("Scale", ImGuiDataType_::ImGuiDataType_Double, &scale, 3))
					{
						entity_transform.setScale(scale);
					};
				}
			}
		}

		if (registry->has<Camera>(entity))
		{
			Camera& camera = registry->get<Camera>(entity);

			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_None))
			{
				ImGui::SliderFloat("fov(x)", &camera.frame_of_view, 5.0f, 140.0f);
				ImGui::InputFloat("z_near", &camera.z_near);
				camera.z_near = std::max(camera.z_near, 0.001f);
				ImGui::InputFloat("z_far", &camera.z_far);
				camera.z_far = std::max(camera.z_near + 1.0f, camera.z_far);
			}
		}

		if (registry->has<Physics::RigidBody>(entity))
		{
			Physics::RigidBody& rigidbody = registry->get<Physics::RigidBody>(entity);

			if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_None))
			{

			}
		}

		if (registry->has<Physics::ProxyShape>(entity))
		{
			Physics::ProxyShape& shape_proxy = registry->get<Physics::ProxyShape>(entity);

			if (ImGui::CollapsingHeader("CollisionShape", ImGuiTreeNodeFlags_None))
			{
				
			}
		}*/
