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

	template<class Component>
	void drawComponent(Entity& entity, const char* component_name, void(*draw_func)(Component&))
	{
		ImGui::PushID(component_name);
		if (entity.hasComponent<Component>())
		{
			bool header_open = ImGui::CollapsingHeader(component_name, ImGuiTreeNodeFlags_DefaultOpen);

			if (ImGui::BeginPopupContextItem(component_name, ImGuiMouseButton_Right))
			{
				if (ImGui::MenuItem("Delete Component"))
				{
					entity.removeComponent<Component>();
					header_open = false;
				}
				ImGui::EndPopup();
			}

			if (header_open)
			{
				draw_func(entity.getComponent<Component>());
			}
		}
		ImGui::PopID();
	};

	//Crappy workaround for not being able to pass arguments or use lamda captures
	//The data variable will just be a pointer to args data
	template<class Component>
	void drawComponentArgs(Entity& entity, const char* component_name, void* data, void(*draw_func)(Component&, void*))
	{
		ImGui::PushID(component_name);
		if (entity.hasComponent<Component>())
		{
			bool header_open = ImGui::CollapsingHeader(component_name, ImGuiTreeNodeFlags_DefaultOpen);

			if (ImGui::BeginPopupContextItem(component_name, ImGuiMouseButton_Right))
			{
				if (ImGui::MenuItem("Delete Component"))
				{
					entity.removeComponent<Component>();
					header_open = false;
				}
				ImGui::EndPopup();
			}

			if (header_open)
			{
				draw_func(entity.getComponent<Component>(), data);
			}
		}
		ImGui::PopID();
	};

	struct Pools
	{
		MeshPool* mesh_pool;
		MaterialPool* material_pool;
	};

#define DRAW_COMPONENT(entity, component, component_name, ...)\
	if (entity.hasComponent<component>()) { bool header_open = ImGui::CollapsingHeader(component_name, ImGuiTreeNodeFlags_DefaultOpen);\
		if (ImGui::BeginPopupContextItem(component_name, ImGuiMouseButton_Right)) {\
			if (ImGui::MenuItem("Delete Component")) { entity.removeComponent<component>(); header_open = false; }\
			ImGui::EndPopup(); }\
			if (header_open) { __VA_ARGS__ ; }}\

#define ADD_COMPONENT(entity, component, component_name, ...) if (!entity.hasComponent<component>() && ImGui::MenuItem(component_name)) { entity.addComponent<component>(__VA_ARGS__); }

	void EntityPropertiesWindow::draw(Entity entity)
	{
		ImGui::Begin("Entity Properties");

		if (entity.valid())
		{
			if (!entity.hasComponent<Node>())
			{
				this->drawEntity(entity);
			}
			else
			{
				this->drawNode(entity);
			}
		}

		ImGui::End();
	}

	void EntityPropertiesWindow::drawEntity(Entity entity)
	{
		if (entity.valid())
		{
			drawComponent<NameComponent>(entity, "Name Component", [](NameComponent& name_component)
			{
				ImGui::InputText("Entity Name", name_component.data, name_component.SIZE);
			});

			drawComponent<TransformD>(entity, "Transform", [](TransformD& transform_component)
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

			drawComponent<Camera>(entity, "Camera", [](Camera& camera_component)
			{
				if (ImGui::DragFloat("Fov X", &camera_component.frame_of_view, 0.5f, 1.0f, 140.0f))
				{
					camera_component.frame_of_view = std::clamp(camera_component.frame_of_view, 1.0f, 140.0f);
				}

				ImGui::InputFloat("Z Near", &camera_component.z_near);
				camera_component.z_near = std::max(camera_component.z_near, 0.001f);
				ImGui::InputFloat("Z Far", &camera_component.z_far);
				camera_component.z_far = std::max(camera_component.z_near + 1.0f, camera_component.z_far);
			});

			drawComponent<DirectionalLight>(entity, "Directional Light", [](DirectionalLight& light_component)
			{
				ImGui::PushID("Directional Light");
				if (ImGui::DragFloat("Intensity", &light_component.intensity, 0.01f, 0.0f, 5.0f))
				{
					light_component.intensity = std::clamp(light_component.intensity, 0.0f, 5.0f);
				}

				ImGui::ColorEdit3("Color", &light_component.color.x, 0);
				ImGui::Checkbox("Enabled", &light_component.enabled);
				ImGui::PopID();
			});

			drawComponent<PointLight>(entity, "Point Light", [](PointLight& light_component)
			{
				ImGui::PushID("Point Light");
				if (ImGui::DragFloat("Intensity", &light_component.intensity, 0.01f, 0.0f, 5.0f))
				{
					light_component.intensity = std::clamp(light_component.intensity, 0.0f, 5.0f);
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

			//The arguments to be passed to the draw lamda
			Pools pools = { this->mesh_pool, this->material_pool };
			drawComponentArgs<ModelComponent>(entity, "Model Component", (void*)&pools, [](ModelComponent& model_component, void* data)
			{
				Pools* pools = (Pools*)data;

				MeshPool* mesh_pool = (MeshPool*)data;

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
							model_component.mesh = pools->mesh_pool->getResource(file_path);
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
							model_component.material = pools->material_pool->getResource(file_path);
						}
					}
				}
			});

			drawComponent<RigidBody>(entity, "Rigid Body", [](RigidBody& rigidbody_component)
			{
				const char* RigidBodyTypeName[] = { "Static", "Kinematic", "Dynamic" };
				RigidBodyType type = rigidbody_component.getType();
				if (ImGui::Combo("Type", (int*)&type, RigidBodyTypeName, IM_ARRAYSIZE(RigidBodyTypeName)))
				{
					GENESIS_INFO("Type changed");
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

				bool awake = rigidbody_component.getIsAllowedToSleep();
				if (ImGui::Checkbox("Is Allowed To Sleep", &awake))
				{
					rigidbody_component.setIsAllowedToSleep(awake);
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

			drawComponent<CollisionShape>(entity, "Collision Shape", [](CollisionShape& shape_component)
			{
				const char* shape_names[] = { "None", "Box", "Sphere", "Capsule" };
				if (ImGui::Combo("Type", (int*)&shape_component.type, shape_names, IM_ARRAYSIZE(shape_names)))
				{
					switch (shape_component.type)
					{
					case CollisionShapeType::Box:
						shape_component.type_data.box_size = vector3D(0.5);
						break;
					case CollisionShapeType::Sphere:
						shape_component.type_data.sphere_radius = 1.0;
						break;
					case CollisionShapeType::Capsule:
						shape_component.type_data.capsule_size = vector2D(0.5, 1.8);
						break;
					}
				}

				if (shape_component.type == CollisionShapeType::Box)
				{
					ImGui::InputScalarN("Half Extents", ImGuiDataType_::ImGuiDataType_Double, &shape_component.type_data.box_size, 3);
				}
				else if (shape_component.type == CollisionShapeType::Sphere)
				{
					ImGui::InputDouble("Radius", &shape_component.type_data.sphere_radius);
				}
				else if (shape_component.type == CollisionShapeType::Capsule)
				{
					ImGui::InputDouble("Radius", &shape_component.type_data.capsule_size.x);
					ImGui::InputDouble("Height", &shape_component.type_data.capsule_size.y);
				}
			});

			ImGui::Separator();

			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("add_component");
			}

			if (ImGui::BeginPopup("add_component"))
			{
				ADD_COMPONENT(entity, NameComponent, "Name Component", "Entity Name");
				ADD_COMPONENT(entity, TransformD, "Transform");
				ADD_COMPONENT(entity, Camera, "Camera");
				ADD_COMPONENT(entity, DirectionalLight, "Directional Light");
				ADD_COMPONENT(entity, PointLight, "Point Light");
				ADD_COMPONENT(entity, ModelComponent, "Model Component");
				ADD_COMPONENT(entity, RigidBody, "RigidBody");
				ADD_COMPONENT(entity, CollisionShape, "Collision Shape");
				ImGui::EndPopup();
			}
		}
	}

	void EntityPropertiesWindow::drawNode(Entity entity)
	{
		if (entity.valid())
		{

			if (ImGui::CollapsingHeader("Node", ImGuiTreeNodeFlags_DefaultOpen))
			{
				Node& node = entity.getComponent<Node>();

				vector3F position = node.local_transform.getPosition();
				if (ImGui::InputFloat3("Position", &position.x, 2))
				{
					node.local_transform.setPosition(position);
				};

				vector3F rotation = glm::degrees(glm::eulerAngles(node.local_transform.getOrientation()));
				if (ImGui::InputFloat3("Rotation", &rotation.x, 2))
				{
					node.local_transform.setOrientation(quaternionD(glm::radians(rotation)));
				}

				vector3F scale = node.local_transform.getScale();
				if (ImGui::InputFloat3("Scale", &scale.x, 1))
				{
					node.local_transform.setScale(scale);
				}
			}

			drawComponent<NameComponent>(entity, "Name Component", [](NameComponent& name_component)
			{
				ImGui::InputText("Node Name", name_component.data, name_component.SIZE);
			});

			drawComponent<Camera>(entity, "Camera", [](Camera& camera_component)
			{
				if (ImGui::DragFloat("Fov X", &camera_component.frame_of_view, 0.5f, 1.0f, 140.0f))
				{
					camera_component.frame_of_view = std::clamp(camera_component.frame_of_view, 1.0f, 140.0f);
				}

				ImGui::InputFloat("Z Near", &camera_component.z_near);
				camera_component.z_near = std::max(camera_component.z_near, 0.001f);
				ImGui::InputFloat("Z Far", &camera_component.z_far);
				camera_component.z_far = std::max(camera_component.z_near + 1.0f, camera_component.z_far);
			});

			drawComponent<DirectionalLight>(entity, "Directional Light", [](DirectionalLight& light_component)
			{
				ImGui::PushID("Directional Light");
				if (ImGui::DragFloat("Intensity", &light_component.intensity, 0.01f, 0.0f, 5.0f))
				{
					light_component.intensity = std::clamp(light_component.intensity, 0.0f, 5.0f);
				}

				ImGui::ColorEdit3("Color", &light_component.color.x, 0);
				ImGui::Checkbox("Enabled", &light_component.enabled);
				ImGui::PopID();
			});

			drawComponent<PointLight>(entity, "Point Light", [](PointLight& light_component)
			{
				ImGui::PushID("Point Light");
				if (ImGui::DragFloat("Intensity", &light_component.intensity, 0.01f, 0.0f, 5.0f))
				{
					light_component.intensity = std::clamp(light_component.intensity, 0.0f, 5.0f);
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

			//The arguments to be passed to the draw lamda
			Pools pools = { this->mesh_pool, this->material_pool };
			drawComponentArgs<ModelComponent>(entity, "Model Component", (void*)&pools, [](ModelComponent& model_component, void* data)
			{
				Pools* pools = (Pools*)data;

				MeshPool* mesh_pool = (MeshPool*)data;

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
							model_component.mesh = pools->mesh_pool->getResource(file_path);
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
							model_component.material = pools->material_pool->getResource(file_path);
						}
					}
				}
			});

			drawComponent<CollisionShape>(entity, "Collision Shape", [](CollisionShape& shape_component)
			{
				const char* shape_names[] = { "None", "Box", "Sphere", "Capsule" };
				if (ImGui::Combo("Type", (int*)&shape_component.type, shape_names, IM_ARRAYSIZE(shape_names)))
				{
					switch (shape_component.type)
					{
					case CollisionShapeType::Box:
						shape_component.type_data.box_size = vector3D(0.5);
						break;
					case CollisionShapeType::Sphere:
						shape_component.type_data.sphere_radius = 1.0;
						break;
					case CollisionShapeType::Capsule:
						shape_component.type_data.capsule_size = vector2D(0.5, 1.8);
						break;
					}
				}

				if (shape_component.type == CollisionShapeType::Box)
				{
					ImGui::InputScalarN("Half Extents", ImGuiDataType_::ImGuiDataType_Double, &shape_component.type_data.box_size, 3);
				}
				else if (shape_component.type == CollisionShapeType::Sphere)
				{
					ImGui::InputDouble("Radius", &shape_component.type_data.sphere_radius);
				}
				else if (shape_component.type == CollisionShapeType::Capsule)
				{
					ImGui::InputDouble("Radius", &shape_component.type_data.capsule_size.x);
					ImGui::InputDouble("Height", &shape_component.type_data.capsule_size.y);
				}
			});

			ImGui::Separator();

			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("add_component");
			}

			if (ImGui::BeginPopup("add_component"))
			{
				ADD_COMPONENT(entity, NameComponent, "Name Component", "Node Name");
				ADD_COMPONENT(entity, Camera, "Camera");
				ADD_COMPONENT(entity, DirectionalLight, "Directional Light");
				ADD_COMPONENT(entity, PointLight, "Point Light");
				ADD_COMPONENT(entity, ModelComponent, "Model Component");
				ADD_COMPONENT(entity, CollisionShape, "Collision Shape");
				ImGui::EndPopup();
			}
		}
	}
}