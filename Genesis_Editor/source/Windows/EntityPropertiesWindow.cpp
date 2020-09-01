#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"

#include "imgui.h"

#include "Genesis/Component/MeshComponent.hpp"
#include "Genesis/Component/TransformComponents.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"

namespace Genesis
{
	EntityPropertiesWindow::EntityPropertiesWindow()
	{
	}

	EntityPropertiesWindow::~EntityPropertiesWindow()
	{
	}

	void EntityPropertiesWindow::drawWindow(EntityRegistry& world, EntityHandle selected_entity)
	{
		ImGui::Begin("Entity Properties");

		if (world.valid(selected_entity))
		{
			if (world.has<NameComponent>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Name Component", ImGuiTreeNodeFlags_DefaultOpen))
				{
					NameComponent& name_component = world.get<NameComponent>(selected_entity);
					ImGui::InputText("Entity Name", name_component.data, name_component.SIZE);
				}
			}

			if (world.has<TransformD>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					TransformD& transform_component = world.get<TransformD>(selected_entity);

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
				}
			}

			if (world.has<Camera>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
				{
					Camera& camera_component = world.get<Camera>(selected_entity);

					ImGui::SliderFloat("Fov X", &camera_component.frame_of_view, 5.0f, 140.0f);
					ImGui::InputFloat("Z Near", &camera_component.z_near);
					camera_component.z_near = std::max(camera_component.z_near, 0.001f);
					ImGui::InputFloat("Z Far", &camera_component.z_far);
					camera_component.z_far = std::max(camera_component.z_near + 1.0f, camera_component.z_far);
				}
			}

			if (world.has<DirectionalLight>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
				{
					DirectionalLight& light_component = world.get<DirectionalLight>(selected_entity);

					ImGui::SliderFloat("Intensity", &light_component.intensity, 0.0f, 1.0f);
					ImGui::ColorEdit3("Color", &light_component.color.x, 0);
					ImGui::Checkbox("Enabled", &light_component.enabled);
				}
			}

			if (world.has<MeshComponent>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Mesh Component", ImGuiTreeNodeFlags_DefaultOpen))
				{

				}
			}

			if (world.has<RigidBody>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
				{
					RigidBody& rigidbody = world.get<RigidBody>(selected_entity);

					double mass = rigidbody.getMass();
					if (ImGui::InputDouble("Mass", &mass))
					{
						rigidbody.setMass(mass);
					}

					bool gravity = rigidbody.getGravityEnabled();
					if (ImGui::Checkbox("Gravity Enabled", &gravity))
					{
						rigidbody.setGravityEnabled(gravity);
					}

					bool awake = rigidbody.getAwake();
					if (ImGui::Checkbox("Awake", &awake))
					{
						rigidbody.setAwake(awake);
					}

					vector3D linear_velocity = rigidbody.getLinearVelocity();
					if (ImGui::InputScalarN("Linear Velocity", ImGuiDataType_::ImGuiDataType_Double, &linear_velocity, 3))
					{
						rigidbody.setLinearVelocity(linear_velocity);
					}

					vector3D angular_velocity = rigidbody.getAngularVelocity();
					if (ImGui::InputScalarN("Angular Velocity", ImGuiDataType_::ImGuiDataType_Double, &angular_velocity, 3))
					{
						rigidbody.setAngularVelocity(angular_velocity);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("add_component");
			}

			if (ImGui::BeginPopup("add_component"))
			{
				if (!world.has<NameComponent>(selected_entity))
				{
					if (ImGui::MenuItem("Name Component")) { world.assign<NameComponent>(selected_entity, "Entity Name"); }
				}

				if (!world.has<TransformD>(selected_entity))
				{
					if (ImGui::MenuItem("Transform")) { world.assign<TransformD>(selected_entity); }
				}

				if (!world.has<Camera>(selected_entity))
				{
					if (!world.has<WorldTransform>(selected_entity))
					{
						world.assign<WorldTransform>(selected_entity);
					}

					if (ImGui::MenuItem("Camera")) { world.assign<Camera>(selected_entity); }
				}

				if (!world.has<MeshComponent>(selected_entity))
				{
					if (!world.has<WorldTransform>(selected_entity))
					{
						world.assign<WorldTransform>(selected_entity);
					}

					if (ImGui::MenuItem("Mesh")) { world.assign<MeshComponent>(selected_entity, nullptr, nullptr); }
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}