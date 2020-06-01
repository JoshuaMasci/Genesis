#include "Genesis/Editor/EntityPropertiesWindow.hpp"

#include "imgui.h"
#include "Genesis/Ecs/NameComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Physics/RigidBody.hpp"

using namespace Genesis;

EntityPropertiesWindow::EntityPropertiesWindow()
{
}

EntityPropertiesWindow::~EntityPropertiesWindow()
{
}

void Genesis::EntityPropertiesWindow::drawWindow(EcsWorld* world, EntityHandle selected_entity)
{
	EntityRegistry* registry = &world->entity_registry;

	ImGui::Begin("Entity Properties");

	if (selected_entity != entt::null)
	{
		if(registry->has<NameComponent>(selected_entity))
		{
			auto& name_component = registry->get<NameComponent>(selected_entity);

			ImGui::Separator();
			ImGui::InputText("Entity Name", name_component.data, name_component.data_size);
		}

		if (registry->has<TransformD>(selected_entity))
		{
			auto& transform_component = registry->get<TransformD>(selected_entity);

			ImGui::Separator();
			if (ImGui::CollapsingHeader("Entity Properties", ImGuiTreeNodeFlags_DefaultOpen))
			{
				vector3D position = transform_component.getPosition();
				if (ImGui::InputScalarN("Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
				{
					transform_component.setPosition(position);
				}

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
			}
		}

		if (registry->has<Camera>(selected_entity))
		{
			auto& camera_component = registry->get<Camera>(selected_entity);
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SliderFloat("fov(x)", &camera_component.frame_of_view, 5.0f, 140.0f);
				ImGui::InputFloat("z_near", &camera_component.z_near);
				camera_component.z_near = std::max(camera_component.z_near, 0.001f);
				ImGui::InputFloat("z_far", &camera_component.z_far);
				camera_component.z_far = std::max(camera_component.z_near + 1.0f, camera_component.z_far);
			}
		}
	}

	ImGui::End();
}

/*void EntityPropertiesWindow::drawWindow(World* world, Entity* selected_entity)
{
	ImGui::ShowDemoWindow();

	ImGui::Begin("Entity Properties");

	if (selected_entity != nullptr)
	{
		ImGui::Separator();
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

		if (selected_entity->getRigidbody() != nullptr)
		{
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
			{
				RigidBody* rigidbody = selected_entity->getRigidbody();

				bool awake = rigidbody->getAwake();
				if (ImGui::Checkbox("Awake", &awake))
				{
					rigidbody->setAwake(awake);
				}

				bool gravity = rigidbody->getGravityEnabled();
				if (ImGui::Checkbox("Gravity Enabled", &gravity))
				{
					rigidbody->setGravityEnabled(gravity);
				}

				vector3D linear_velocity = rigidbody->getLinearVelocity();
				if (ImGui::InputScalarN("Linear Velocity", ImGuiDataType_::ImGuiDataType_Double, &linear_velocity, 3))
				{
					rigidbody->setLinearVelocity(linear_velocity);
				};

				vector3D angular_velocity = rigidbody->getAngularVelocity();
				if (ImGui::InputScalarN("Angular Velocity", ImGuiDataType_::ImGuiDataType_Double, &angular_velocity, 3))
				{
					rigidbody->setLinearVelocity(angular_velocity);
				};
			}
		}

		ImGui::Separator();

		/*auto size = ImGui::GetContentRegionAvail();
		size.y = 19;
		if (ImGui::Button("Add component", size))
		{
			ImGui::OpenPopup("add_component");
		}

		if (ImGui::BeginPopup("add_component"))
		{
			if (ImGui::Selectable("Rigidbody"))
			{
				//selected_entity->addRigidbody();
			}

			if (ImGui::Selectable("Box Collider"))
			{
				//selected_entity->addCollisionShape((CollisionShape*)new BoxCollisionShape(vector3D(0.5)));
			}

			ImGui::EndPopup();
		}
	}
	else
	{
		ImGui::TextUnformatted("Error: Invalid Entity");
	}

	ImGui::End();
}
*/
