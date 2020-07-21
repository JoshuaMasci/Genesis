#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"

#include "imgui.h"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"

#include "Genesis/Resource/PbrMesh.hpp"
#include "Genesis/Resource/PbrMaterial.hpp"

namespace Genesis
{
	EntityPropertiesWindow::EntityPropertiesWindow()
	{
	}

	EntityPropertiesWindow::~EntityPropertiesWindow()
	{
	}

	void EntityPropertiesWindow::drawWindow(EntityRegistry* registry, EntityWorld* world, EntityId selected_entity)
	{
		ImGui::Begin("Entity Properties");

		if (selected_entity != InvalidEntity)
		{
			if (world->hasComponent<NameComponent>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Name Component", ImGuiTreeNodeFlags_DefaultOpen))
				{
					NameComponent* name_component = world->getComponent<NameComponent>(selected_entity);
					ImGui::InputText("Entity Name", name_component->data, name_component->SIZE);
				}
			}

			if (world->hasComponent<TransformD>(selected_entity))
			{
				if (ImGui::CollapsingHeader("World Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					TransformD& transform_component = *world->getComponent<TransformD>(selected_entity);

					vector3D position = transform_component.getPosition();
					if (ImGui::InputScalarN("World Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
					{
						transform_component.setPosition(position);
					};

					vector3D rotation = glm::degrees(glm::eulerAngles(transform_component.getOrientation()));
					if (ImGui::InputScalarN("World Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
					{
						transform_component.setOrientation(quaternionD(glm::radians(rotation)));
					}

					vector3D scale = transform_component.getScale();
					if (ImGui::InputScalarN("World Scale", ImGuiDataType_::ImGuiDataType_Double, &scale, 3))
					{
						transform_component.setScale(scale);
					}
				}
			}

			if (world->hasComponent<Camera>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
				{
					Camera& camera_component = *world->getComponent<Camera>(selected_entity);

					ImGui::SliderFloat("Fov X", &camera_component.frame_of_view, 5.0f, 140.0f);
					ImGui::InputFloat("Z Near", &camera_component.z_near);
					camera_component.z_near = std::max(camera_component.z_near, 0.001f);
					ImGui::InputFloat("Z Far", &camera_component.z_far);
					camera_component.z_far = std::max(camera_component.z_near + 1.0f, camera_component.z_far);
				}
			}

			if (world->hasComponent<DirectionalLight>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
				{
					DirectionalLight& light_component = *world->getComponent<DirectionalLight>(selected_entity);

					ImGui::SliderFloat("Intensity", &light_component.intensity, 0.0f, 1.0f);
					ImGui::ColorEdit3("Color", &light_component.color.x, 0);
					ImGui::Checkbox("Enabled", &light_component.enabled);
				}
			}

			if (world->hasComponent<PbrMaterial>(selected_entity))
			{
				if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
				{
					PbrMaterial& material_component = *world->getComponent<PbrMaterial>(selected_entity);

					ImGui::ColorEdit4("Albedo Color", &material_component.albedo_factor.x, 0);
					ImGui::SliderFloat("Metallic Factor", &material_component.metallic_roughness_factor.x, 0.0f, 1.0f);
					ImGui::SliderFloat("Roughness Factor", &material_component.metallic_roughness_factor.y, 0.0f, 1.0f);
					ImGui::ColorEdit4("Emissive Color", &material_component.emissive_factor.x, 0);

					//ImGui::SliderFloat("Intensity", &light_component.intensity, 0.0f, 1.0f);
					//ImGui::ColorEdit3("Color", &light_component.color.x, 0);
					//ImGui::Checkbox("Enabled", &light_component.enabled);

					ImGui::Checkbox("Cull Backface", &material_component.cull_backface);
				}
			}

		}

		ImGui::End();
	}

	/*void Genesis::EntityPropertiesWindow::drawWindow()
	{
		ImGui::Begin("Entity Properties");

		if (selected_entity != nullptr)
		{
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Entity Properties", ImGuiTreeNodeFlags_DefaultOpen))
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
					TransformD local_transform = selected_entity->getWorldTransform();

					vector3D position = local_transform.getPosition();
					if (ImGui::InputScalarN("World Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
					{
						local_transform.setPosition(position);
						has_changed_transform = true;
					};

					vector3D rotation = glm::degrees(glm::eulerAngles(local_transform.getOrientation()));
					if (ImGui::InputScalarN("World Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
					{
						local_transform.setOrientation(quaternionD(glm::radians(rotation)));
						has_changed_transform = true;
					}

					vector3D scale = local_transform.getScale();
					if (ImGui::InputScalarN("World Scale", ImGuiDataType_::ImGuiDataType_Double, &scale, 3))
					{
						local_transform.setScale(scale);
						has_changed_transform = true;
					}

					if (has_changed_transform)
					{
						selected_entity->setWorldTransform(local_transform);
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
		}

		ImGui::End();
	}*/
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
