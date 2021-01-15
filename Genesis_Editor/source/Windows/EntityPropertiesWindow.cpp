#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"

#include "imgui.h"
#include "Genesis/Platform/FileSystem.hpp"

#include "Genesis/Resource/ResourceManager.hpp"

#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/TransformComponent.hpp"
#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
#include "Genesis/Physics/PhysicsWorld.hpp"

#include "Genesis/Scene/Entity.hpp"
#include "Genesis/Scene/ScenePrototype.hpp"

namespace Genesis
{
	EntityPropertiesWindow::EntityPropertiesWindow(ResourceManager* resource_manager)
	{
		this->resource_manager = resource_manager;
	}

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
	bool drawComponent2(Entity& entity, const char* component_name)
	{
		bool header_open = false;

		if (entity.has<Component>())
		{
			if (ImGui::BeginPopupContextItem(component_name, ImGuiMouseButton_Right))
			{
				if (ImGui::MenuItem("Delete Component"))
				{
					entity.remove<Component>();
					header_open = false;
				}
				ImGui::EndPopup();
			}
		}

		return header_open;
	}

#define ADD_COMPONENT(entity, component, component_name, ...) if (!entity.has<component>() && ImGui::MenuItem(component_name)) { entity.add<component>(__VA_ARGS__); }

	void EntityPropertiesWindow::draw(Entity entity)
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
						model_component.mesh = this->resource_manager->mesh_pool.getResource(file_path);
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
						model_component.material = this->resource_manager->material_pool.getResource(file_path);
					}
				}
			}
		});

		drawComponent<DirectionalLight>(entity, "Directional Light", [=](DirectionalLight& light_component)
		{
			ImGui::PushID("Directional Light");

			if (ImGui::DragFloat("Intensity", &light_component.intensity, 0.01f, 0.0f, 1.0f))
			{
				light_component.intensity = std::clamp(light_component.intensity, 0.0f, 1.0f);
			}

			ImGui::ColorEdit3("Color", &light_component.color.x, 0);
			ImGui::Checkbox("Enabled", &light_component.enabled);
			ImGui::PopID();
		});

		drawComponent<PointLight>(entity, "Point Light", [=](PointLight& light_component)
		{
			ImGui::PushID("Point Light");

			if (ImGui::DragFloat("Intensity", &light_component.intensity, 0.01f, 0.0f, 1.0f))
			{
				light_component.intensity = std::clamp(light_component.intensity, 0.0f, 1.0f);
			}

			ImGui::ColorEdit3("Color", &light_component.color.x, 0);

			if (ImGui::DragFloat("Range", &light_component.range, 0.25f, 0.25f, 1000.0f))
			{
				light_component.range = std::clamp(light_component.range, 0.01f, 1000.0f);
			}

			if (ImGui::DragFloat2("Attenuation", &light_component.attenuation.x, 0.01f, 0.0f, 1.0f))
			{
				light_component.attenuation.x = std::clamp(light_component.attenuation.x, 0.0f, 1.0f);
				light_component.attenuation.y = std::clamp(light_component.attenuation.y, 0.0f, 1.0f);
			}

			ImGui::Checkbox("Enabled", &light_component.enabled);

			ImGui::PopID();
		});

		drawComponent<RigidBody>(entity, "Rigidbody", [](RigidBody& rigidbody_component)
		{
			RigidBodyType type = rigidbody_component.getType();
			const char* type_names[] = { "Static", "Kinematic", "Dynamic" };
			if (ImGui::Combo("Type", (int*)&type, type_names, IM_ARRAYSIZE(type_names)))
			{
				rigidbody_component.setType(type);
			}

			double mass = rigidbody_component.getMass();
			if (ImGui::InputDouble("Mass", &mass))
			{
				rigidbody_component.setMass(mass);
			}

			bool gravity = rigidbody_component.getGravityEnabled();
			if (ImGui::Checkbox("Gravity Enabled", &gravity))
			{
				rigidbody_component.setGravityEnabled(gravity);
			}

			bool is_allowed_to_sleep = rigidbody_component.getIsAllowedToSleep();
			if (ImGui::Checkbox("Is allowed to sleep", &is_allowed_to_sleep))
			{
				rigidbody_component.setIsAllowedToSleep(is_allowed_to_sleep);
			}

			vector3D linear_velocity = rigidbody_component.getLinearVelocity();
			if (ImGui::InputScalarN("Linear Velocity", ImGuiDataType_::ImGuiDataType_Double, &linear_velocity, 3))
			{
				rigidbody_component.setLinearVelocity(linear_velocity);
			}

			vector3D angular_velocity = rigidbody_component.getAngularVelocity();
			if (ImGui::InputScalarN("Angular Velocity", ImGuiDataType_::ImGuiDataType_Double, &angular_velocity, 3))
			{
				rigidbody_component.setAngularVelocity(angular_velocity);
			}
		});

		drawComponent<CollisionShape>(entity, "Collision Shape", [](CollisionShape& collision_shape) 
		{
			const char* shape_names[] = { "None", "Box", "Sphere", "Capsule" };
			if (ImGui::Combo("Shape", (int*)&collision_shape.type, shape_names, IM_ARRAYSIZE(shape_names)))
			{
				switch (collision_shape.type)
				{
				case CollisionShapeType::Box:
					collision_shape.type_data.box_size = vector3D(0.5);
					break;
				case CollisionShapeType::Sphere:
					collision_shape.type_data.sphere_radius = 0.5;
					break;
				case CollisionShapeType::Capsule:
					collision_shape.type_data.capsule_size = vector2D(0.5, 1.8);
					break;
				}
			}

			if (collision_shape.type == CollisionShapeType::Box)
			{
				ImGui::InputScalarN("Half Extents", ImGuiDataType_::ImGuiDataType_Double, &collision_shape.type_data.box_size, 3);
			}
			else if (collision_shape.type == CollisionShapeType::Sphere)
			{
				ImGui::InputDouble("Radius", &collision_shape.type_data.sphere_radius);
			}
			else if (collision_shape.type == CollisionShapeType::Capsule)
			{
				ImGui::InputDouble("Radius", &collision_shape.type_data.capsule_size.x);
				ImGui::InputDouble("Height", &collision_shape.type_data.capsule_size.y);
			}
		});

		ImGui::Separator();

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("add_component");
		}

		if (ImGui::BeginPopup("add_component"))
		{
			Scene* scene = entity.get_scene();
			ADD_COMPONENT(entity, NameComponent, "Name", "Unamed Entity");
			ADD_COMPONENT(entity, Transform, "Transform");
			ADD_COMPONENT(entity, ModelComponent, "Model");
			ADD_COMPONENT(entity, DirectionalLight, "Directional Light");
			ADD_COMPONENT(entity, PointLight, "Point Light");
			ADD_COMPONENT(entity, RigidBody, "RigidBody");
			ADD_COMPONENT(entity, CollisionShape, "Collision Shape");
			ADD_COMPONENT(entity, WorldTransform, "WorldTransform");

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	template<class Component>
	void draw_component(EntityPrototype* entity, const char* component_name, function<void(Component&)> draw_function)
	{
		if (entity.has<Component>())
		{
			bool header_open = ImGui::CollapsingHeader(component_name, ImGuiTreeNodeFlags_DefaultOpen);

			if (ImGui::BeginPopupContextItem(component_name, ImGuiMouseButton_Right))
			{
				if (ImGui::MenuItem("Delete Component"))
				{
					entity->remove_component<Component>();
					header_open = false;
				}
				ImGui::EndPopup();
			}

			if (header_open)
			{
				ImGui::PushID(component_name);
				draw_function(*entity->get_component<Component>());
				ImGui::PopID();
			}
		}
	};

	void EntityPropertiesWindow::draw(EntityPrototype* entity)
	{
		ImGui::Begin("Entity Properties");

		if (!entity)
		{
			ImGui::End();
			return;
		}

		if (ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_DefaultOpen))
		{
			NameComponent name(entity->get_name().c_str());
			ImGui::InputText("Entity Name", name.data, name.SIZE);

			TransformD& transform = entity->get_transform();

			vector3D position = transform.getPosition();
			if (ImGui::InputScalarN("Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
			{
				transform.setPosition(position);
			};

			vector3D rotation = glm::degrees(glm::eulerAngles(transform.getOrientation()));
			if (ImGui::InputScalarN("Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
			{
				transform.setOrientation(quaternionD(glm::radians(rotation)));
			}

			vector3D scale = transform.getScale();
			if (ImGui::InputScalarN("Scale", ImGuiDataType_::ImGuiDataType_Double, &scale, 3))
			{
				transform.setScale(scale);
			}
		}


		/*drawComponent<NameComponent>(entity, "Name Component", [](NameComponent& name_component)
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
		});*/

		ImGui::End();
	}

}