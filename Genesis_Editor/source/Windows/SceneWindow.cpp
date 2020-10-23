#include "Genesis_Editor/Windows/SceneWindow.hpp"

#include "imgui.h"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Component/NodeComponent.hpp"

namespace Genesis
{
	SceneWindow::SceneWindow(InputManager* input_manager, LegacyBackend* legacy_backend)
	{
		this->input_manager = input_manager;

		this->legacy_backend = legacy_backend;
		this->world_renderer = new LegacyWorldRenderer(this->legacy_backend);

		this->scene_camera_transform.setPosition(vector3D(0.0, 0.0, -5.0));

	}

	SceneWindow::~SceneWindow()
	{
		delete this->world_renderer;
		if (this->framebuffer != nullptr)
		{
			this->legacy_backend->destoryFramebuffer(this->framebuffer);
		}
	}

	constexpr fnv_hash32 debug_forward_axis = StringHash32("Debug_ForwardBackward");
	constexpr fnv_hash32 debug_forward = StringHash32("Debug_Forward");
	constexpr fnv_hash32 debug_backward = StringHash32("Debug_Backward");

	constexpr fnv_hash32 debug_up_axis = StringHash32("Debug_UpDown");
	constexpr fnv_hash32 debug_up = StringHash32("Debug_Up");
	constexpr fnv_hash32 debug_down = StringHash32("Debug_Down");

	constexpr fnv_hash32 debug_left_axis = StringHash32("Debug_LeftRight");
	constexpr fnv_hash32 debug_left = StringHash32("Debug_Left");
	constexpr fnv_hash32 debug_right = StringHash32("Debug_Right");

	constexpr fnv_hash32 debug_pitch_axis = StringHash32("Debug_Pitch");
	constexpr fnv_hash32 debug_pitch_up = StringHash32("Debug_PitchUp");
	constexpr fnv_hash32 debug_pitch_down = StringHash32("Debug_PitchDown");

	constexpr fnv_hash32 debug_yaw_axis = StringHash32("Debug_Yaw");
	constexpr fnv_hash32 debug_yaw_left = StringHash32("Debug_YawLeft");
	constexpr fnv_hash32 debug_yaw_right = StringHash32("Debug_YawRight");

	constexpr fnv_hash32 debug_roll_axis = StringHash32("Debug_Roll");
	constexpr fnv_hash32 debug_roll_left = StringHash32("Debug_RollLeft");
	constexpr fnv_hash32 debug_roll_right = StringHash32("Debug_RollRight");

	void SceneWindow::update(TimeStep time_step)
	{		
		if (this->is_window_active && this->override_camera == null_entity)
		{
			vector3D position = this->scene_camera_transform.getPosition();
			position += (this->scene_camera_transform.getForward() * (double)this->input_manager->getButtonAxis(debug_forward_axis, debug_forward, debug_backward)) * this->linear_speed * time_step;
			position += (this->scene_camera_transform.getUp() * (double)this->input_manager->getButtonAxis(debug_up_axis, debug_up, debug_down)) * this->linear_speed * time_step;
			position += (this->scene_camera_transform.getLeft() * (double)this->input_manager->getButtonAxis(debug_left_axis, debug_left, debug_right)) * this->linear_speed * time_step;
			this->scene_camera_transform.setPosition(position);

			quaternionD orientation = this->scene_camera_transform.getOrientation();
			orientation = glm::angleAxis((double)this->input_manager->getButtonAxis(debug_pitch_axis, debug_pitch_up, debug_pitch_down) * this->angular_speed * (PI_D * 2.0) * time_step, this->scene_camera_transform.getLeft()) * orientation;
			orientation = glm::angleAxis((double)this->input_manager->getButtonAxis(debug_yaw_axis, debug_yaw_left, debug_yaw_right) * this->angular_speed * (PI_D * 2.0) * time_step, this->scene_camera_transform.getUp()) * orientation;
			orientation = glm::angleAxis((double)this->input_manager->getButtonAxis(debug_roll_axis, debug_roll_left, debug_roll_right) * this->angular_speed * (PI_D) * time_step, -this->scene_camera_transform.getForward()) * orientation;
			this->scene_camera_transform.setOrientation(orientation);
		}
	}

	void SceneWindow::draw(EntityWorld& world)
	{
		ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (!this->is_scene_running)
			{
				if (ImGui::MenuItem("Play"))
				{
					this->is_scene_running = true;
					world.onCreate();
				}
			}
			else
			{
				if (ImGui::MenuItem("Pause"))
				{
					this->is_scene_running = false;
					world.onDestroy();
				}
			}

			if (ImGui::BeginMenu("Graphics"))
			{
				static bool lighting = true;
				if (ImGui::MenuItem("Lighting", nullptr, lighting))
				{
					lighting = !lighting;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Camera"))
			{
				if (ImGui::MenuItem("Default", nullptr, this->override_camera == null_entity))
				{
					this->override_camera = null_entity;
				}

				auto* registry = world.getRegistry();

				//Entity Cameras
				{
					auto camera_view = registry->view<Camera>();
					for (auto entity : camera_view)
					{
						char* name = "Unnamed Entity";
						NameComponent* name_component = registry->try_get<NameComponent>(entity);
						if (name_component != nullptr)
						{
							name = name_component->data;
						}

						if (ImGui::MenuItem(name, nullptr, this->override_camera == entity && this->override_camera_node == null_entity))
						{
							this->override_camera = entity;
							this->override_camera_node = null_entity;
						}
					}
				}
				
				//Node Cameras
				{
					bool display_seperator = true;
					auto node_component_view = registry->view<NodeComponent>();
					for (auto entity : node_component_view)
					{
						NodeComponent& node_component = node_component_view.get<NodeComponent>(entity);
		
						auto camera_view = node_component.registry.view<Camera>();
						for (auto node : camera_view)
						{
							char* name = "Unnamed Node";
							NameComponent* name_component = node_component.registry.try_get<NameComponent>(node);
							if (name_component != nullptr)
							{
								name = name_component->data;
							}

							if (display_seperator)
							{
								ImGui::Separator();
								display_seperator = false;
							}

							if (ImGui::MenuItem(name, nullptr, this->override_camera == entity && this->override_camera_node == node))
							{
								this->override_camera = entity;
								this->override_camera_node = node;
							}
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImVec2 im_remaining_space = ImGui::GetContentRegionAvail();
		vector2U window_size = vector2U(im_remaining_space.x, im_remaining_space.y);

		if (window_size != this->framebuffer_size)
		{
			//Rebuild Framebuffer
			if (this->framebuffer != nullptr)
			{
				this->legacy_backend->destoryFramebuffer(this->framebuffer);
			}

			this->framebuffer_size = window_size;
			FramebufferAttachmentInfo color_attachment = { ImageFormat::RGBA_32_Float, MultisampleCount::Sample_1 };
			FramebufferDepthInfo depth_attachment = { DepthFormat::depth_24,  MultisampleCount::Sample_1 };
			FramebufferCreateInfo create_info = {};
			create_info.attachments = &color_attachment;
			create_info.attachment_count = 1;
			create_info.depth_attachment = &depth_attachment;
			create_info.size = this->framebuffer_size;

			this->framebuffer = this->legacy_backend->createFramebuffer(create_info);
		}

		EntityRegistry* registry = world.getRegistry();

		if (this->override_camera != null_entity && this->override_camera_node == null_entity)
		{
			Camera& camera = registry->get<Camera>(this->override_camera);
			TransformD& camera_transform = registry->get<TransformD>(this->override_camera);
			this->world_renderer->drawScene(this->framebuffer_size, this->framebuffer, *registry, camera, camera_transform);
		}
		else if (this->override_camera != null_entity && this->override_camera_node != null_entity)
		{
			TransformD& entity_transform = registry->get<TransformD>(this->override_camera);
			NodeComponent& node_component = registry->get<NodeComponent>(this->override_camera);

			Node& node = node_component.registry.get<Node>(this->override_camera_node);
			Camera& camera = node_component.registry.get<Camera>(this->override_camera_node);

			TransformD camera_transform;
			TransformUtils::transformByInplace(camera_transform, entity_transform, node.entity_space_transform);
			this->world_renderer->drawScene(this->framebuffer_size, this->framebuffer, *registry, camera, camera_transform);
		}
		else
		{
			this->world_renderer->drawScene(this->framebuffer_size, this->framebuffer, *registry, this->scene_camera, this->scene_camera_transform);
		}

		ImGui::Image((ImTextureID)this->legacy_backend->getFramebufferColorAttachment(this->framebuffer, 0), im_remaining_space, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		this->is_window_active = ImGui::IsWindowFocused();

		ImGui::End();
	}
}