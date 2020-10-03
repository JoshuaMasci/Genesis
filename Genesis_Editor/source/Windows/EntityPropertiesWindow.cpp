#include "Genesis_Editor/Windows/EntityPropertiesWindow.hpp"

#include "imgui.h"
#include "Genesis/Platform/FileSystem.hpp"

#include "Genesis/Ecs/Entity.hpp"

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

	void EntityPropertiesWindow::draw(EntityRegistry& registry, EntityHandle selected_entity)
	{
		Entity entity(selected_entity, &registry);

		ImGui::Begin("Entity Properties");

		if (entity.valid())
		{
			if (entity.hasComponent<NameComponent>())
			{
				if (ImGui::CollapsingHeader("Name Component", ImGuiTreeNodeFlags_DefaultOpen))
				{
					NameComponent& name_component = entity.getComponent<NameComponent>();
					ImGui::InputText("Entity Name", name_component.data, name_component.SIZE);
				}
			}

			if (entity.hasComponent<TransformD>())
			{
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					TransformD& transform_component = entity.getComponent<TransformD>();

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

			if (entity.hasComponent<Camera>())
			{
				if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
				{
					Camera& camera_component = entity.getComponent<Camera>();

					if (ImGui::DragFloat("Fov X", &camera_component.frame_of_view, 0.5f, 1.0f, 140.0f))
					{
						camera_component.frame_of_view = std::clamp(camera_component.frame_of_view, 1.0f, 140.0f);
					}

					ImGui::InputFloat("Z Near", &camera_component.z_near);
					camera_component.z_near = std::max(camera_component.z_near, 0.001f);
					ImGui::InputFloat("Z Far", &camera_component.z_far);
					camera_component.z_far = std::max(camera_component.z_near + 1.0f, camera_component.z_far);
				}
			}

			if (entity.hasComponent<DirectionalLight>())
			{
				ImGui::PushID("Directional Light");
				if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
				{
					DirectionalLight& light_component = entity.getComponent<DirectionalLight>();

					if (ImGui::DragFloat("Intensity", &light_component.intensity, 0.01f, 0.0f, 1.0f))
					{
						light_component.intensity = std::clamp(light_component.intensity, 0.0f, 1.0f);
					}

					ImGui::ColorEdit3("Color", &light_component.color.x, 0);
					ImGui::Checkbox("Enabled", &light_component.enabled);
				}
				ImGui::PopID();
			}

			if (entity.hasComponent<PointLight>())
			{
				ImGui::PushID("Point Light");
				if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
				{
					PointLight& light_component = entity.getComponent<PointLight>();

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
				}
				ImGui::PopID();
			}

			if (entity.hasComponent<ModelComponent>())
			{
				if (ImGui::CollapsingHeader("Model Component", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::OpenPopupOnItemClick("Delete_Component", ImGuiMouseButton_Right);

					ModelComponent& model = entity.getComponent<ModelComponent>();

					const char* mesh_name = " ";

					if (model.mesh)
					{
						mesh_name = model.mesh->getName().c_str();
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
								model.mesh = this->mesh_pool->getResource(file_path);
							}
						}
					}

					const char* material_name = " ";

					if (model.material)
					{
						material_name = model.material->getName().c_str();
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
								model.material = this->material_pool->getResource(file_path);
							}
						}
					}
				}
				else
				{
					ImGui::OpenPopupOnItemClick("Delete_Component", ImGuiMouseButton_Right);
				}

				if (ImGui::BeginPopup("Delete_Component"))
				{
					if (ImGui::MenuItem("Delete Component")) { entity.removeComponent<ModelComponent>(); }
					ImGui::EndPopup();
				}
			}

			if (entity.hasComponent<RigidBody>())
			{
				if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
				{
					RigidBody& rigidbody = entity.getComponent<RigidBody>();

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

			if (entity.hasComponent<CollisionShape>())
			{
				if (ImGui::CollapsingHeader("Collision Shape", ImGuiTreeNodeFlags_DefaultOpen))
				{
					CollisionShape& shape = entity.getComponent<CollisionShape>();

					const char* shape_names[] = { "None", "Box", "Sphere", "Capsule" };
					ImGui::Combo("Type", (int*)&shape.type, shape_names, IM_ARRAYSIZE(shape_names));

					if (shape.type == CollisionShapeType::Box)
					{
						ImGui::InputScalarN("Half Extents", ImGuiDataType_::ImGuiDataType_Double, &shape.type_data.box_size, 3);
					}
					else if (shape.type == CollisionShapeType::Sphere)
					{
						ImGui::InputDouble("Radius", &shape.type_data.sphere_radius);
					}
					else if (shape.type == CollisionShapeType::Capsule)
					{
						ImGui::InputDouble("Radius", &shape.type_data.capsule_size.x);
						ImGui::InputDouble("Height", &shape.type_data.capsule_size.y);
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
				if (!entity.hasComponent<NameComponent>())
				{
					if (ImGui::MenuItem("Name Component")) { entity.addComponent<NameComponent>("Entity Name"); }
				}

				if (!entity.hasComponent<TransformD>())
				{
					if (ImGui::MenuItem("Transform")) { entity.addComponent<TransformD>(); }
				}

				if (!entity.hasComponent<Camera>())
				{
					if (ImGui::MenuItem("Camera")) { entity.addComponent<Camera>(); }
				}

				if (!entity.hasComponent<ModelComponent>())
				{
					if (ImGui::MenuItem("Model Component")) { entity.addComponent<ModelComponent>(nullptr, nullptr); }
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}