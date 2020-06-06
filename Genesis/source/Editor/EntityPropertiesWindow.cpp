#include "Genesis/Editor/EntityPropertiesWindow.hpp"

#include "imgui.h"
#include "Genesis/Entity/Entity.hpp"
#include "Genesis/Physics/RigidBody.hpp"

namespace Genesis
{
	EntityPropertiesWindow::EntityPropertiesWindow()
	{
	}

	EntityPropertiesWindow::~EntityPropertiesWindow()
	{
	}

	void Genesis::EntityPropertiesWindow::drawWindow(Entity* selected_entity)
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
	}
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
