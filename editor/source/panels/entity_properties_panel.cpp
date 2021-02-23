#include "genesis_editor/panels/entity_properties_panel.hpp"

#include "imgui.h"
#include "genesis_engine/platform/file_system.hpp"

#include "genesis_engine/resource/resource_manager.hpp"

#include "genesis_engine/component/name_component.hpp"
#include "genesis_engine/component/transform_component.hpp"
#include "genesis_engine/component/model_component.hpp"
#include "genesis_engine/component/physics_components.hpp"
#include "genesis_engine/rendering/camera.hpp"
#include "genesis_engine/rendering/lights.hpp"

#include "genesis_engine/scene/entity.hpp"

namespace genesis
{
	EntityPropertiesWindow::EntityPropertiesWindow(ResourceManager* resource_manager)
	{
		this->resource_manager = resource_manager;
	}

	template<class Component>
	void draw_component(Entity& entity, const char* component_name, function<void(Component&)> draw_function)
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
	}

	template<typename T, typename... TArgs> 
	void add_component(Entity& entity, const char* component_name, TArgs&&... mArgs)
	{
		if (!entity.has<T>() && ImGui::MenuItem(component_name))
		{
			entity.add<T>(std::forward<TArgs>(mArgs)...);
		}
	}

	void EntityPropertiesWindow::draw(Entity entity)
	{
		ImGui::Begin("Entity Properties");

		if (!entity.valid())
		{
			ImGui::End();
			return;
		}

		draw_component<NameComponent>(entity, "Name Component", [](NameComponent& name_component)
		{
			ImGui::InputText("Entity Name", name_component.data, name_component.SIZE);
		});

		draw_component<Transform>(entity, "Transform", [](Transform& transform_component)
		{
			vec3d position = transform_component.getPosition();
			if (ImGui::InputScalarN("Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
			{
				transform_component.setPosition(position);
			};

			vec3d rotation = glm::degrees(glm::eulerAngles(transform_component.getOrientation()));
			if (ImGui::InputScalarN("Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
			{
				transform_component.setOrientation(quatd(glm::radians(rotation)));
			}

			vec3d scale = transform_component.getScale();
			if (ImGui::InputScalarN("Scale", ImGuiDataType_::ImGuiDataType_Double, &scale, 3))
			{
				transform_component.setScale(scale);
			}
		});

		draw_component<ModelComponent>(entity, "Model Component", [=](ModelComponent& model_component)
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

		draw_component<DirectionalLight>(entity, "Directional Light", [=](DirectionalLight& light_component)
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

		draw_component<PointLight>(entity, "Point Light", [=](PointLight& light_component)
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

		draw_component<RigidBodyTemplate>(entity, "Rigidbody", [](RigidBodyTemplate& rigidbody_component)
		{
			const char* type_names[] = { "Static", "Kinematic", "Dynamic" };
			ImGui::Combo("Type", (int*)&rigidbody_component.type, type_names, IM_ARRAYSIZE(type_names));
			ImGui::InputDouble("Mass", &rigidbody_component.mass);
			ImGui::Checkbox("Gravity Enabled", &rigidbody_component.gravity_enabled);
			ImGui::Checkbox("Is allowed to sleep", &rigidbody_component.is_allowed_to_sleep);
			ImGui::InputScalarN("Linear Velocity", ImGuiDataType_::ImGuiDataType_Double, &rigidbody_component.linear_velocity, 3);
			ImGui::InputScalarN("Angular Velocity", ImGuiDataType_::ImGuiDataType_Double, &rigidbody_component.angular_velocity, 3);
		});

		draw_component<CollisionShapeTemplate>(entity, "Collision Shape", [](CollisionShapeTemplate& collision_shape)
		{
			const char* shape_names[] = { "None", "Box", "Sphere", "Capsule" };
			if (ImGui::Combo("Shape", (int*)&collision_shape.type, shape_names, IM_ARRAYSIZE(shape_names)))
			{
				switch (collision_shape.type)
				{
				case ShapeType::Box:
					collision_shape.type_data.box_size = vec3d(0.5);
					break;
				case ShapeType::Sphere:
					collision_shape.type_data.sphere_radius = 0.5;
					break;
				case ShapeType::Capsule:
					collision_shape.type_data.capsule_size = vec2d(0.5, 1.8);
					break;
				}
			}

			if (collision_shape.type == ShapeType::Box)
			{
				ImGui::InputScalarN("Half Extents", ImGuiDataType_::ImGuiDataType_Double, &collision_shape.type_data.box_size, 3);
			}
			else if (collision_shape.type == ShapeType::Sphere)
			{
				ImGui::InputDouble("Radius", &collision_shape.type_data.sphere_radius);
			}
			else if (collision_shape.type == ShapeType::Capsule)
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
			add_component<NameComponent>(entity, "Name", "Unamed Entity");
			add_component<Transform>(entity, "Transform");
			add_component<ModelComponent>(entity, "Model");
			add_component<DirectionalLight>(entity, "Directional Light");
			add_component<PointLight>(entity, "Point Light");
			add_component<RigidBodyTemplate>(entity, "RigidBody");
			add_component<CollisionShapeTemplate>(entity, "Collision Shape");
			ImGui::EndPopup();
		}

		ImGui::End();
	}
}