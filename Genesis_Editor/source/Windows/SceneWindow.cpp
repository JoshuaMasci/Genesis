#include "Genesis_Editor/Windows/SceneWindow.hpp"

#include "imgui.h"

namespace Genesis
{
	SceneWindow::SceneWindow(InputManager* input_manager, LegacyBackend* legacy_backend)
	{
		this->input_manager = input_manager;

		this->legacy_backend = legacy_backend;
		this->world_renderer = new LegacySceneRenderer(this->legacy_backend);

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
		if (this->window_active)
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

	void SceneWindow::draw(SceneRenderList& render_list, SceneLightingSettings& lighting)
	{
		ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Graphics"))
			{
				ImGui::MenuItem("Lighting Enabled", nullptr, &this->settings.lighting);
				ImGui::MenuItem("Frustrum Culling", nullptr, &this->settings.frustrum_culling);
				ImGui::Separator();
				ImGui::Text("Gamma Correction:");
				ImGui::SliderFloat("##Gamma Correction:", &lighting.gamma_correction, 1.0f, 5.0f, "%.2f");
				ImGui::ColorEdit3("Ambient Light", &lighting.ambient_light.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Camera"))
			{
				if (ImGui::MenuItem("Default", nullptr, true))
				{
				}

				ImGui::Separator();
				if (ImGui::BeginMenu("Default Camera"))
				{
					ImGui::Text("Fov X:");
					if (ImGui::DragFloat("##Fov_X", &this->scene_camera.frame_of_view, 0.5f, 1.0f, 140.0f))
					{
						this->scene_camera.frame_of_view = std::clamp(this->scene_camera.frame_of_view, 1.0f, 140.0f);
					}

					ImGui::Text("Z Near:");
					if (ImGui::InputFloat("##Z_Near", &this->scene_camera.z_near))
					{
						this->scene_camera.z_near = std::max(this->scene_camera.z_near, 0.001f);
					}

					ImGui::Text("Z Far:");
					if (ImGui::InputFloat("##Z_Far", &this->scene_camera.z_far))
					{
						this->scene_camera.z_far = std::max(this->scene_camera.z_near + 1.0f, this->scene_camera.z_far);
					}
					ImGui::Separator();

					ImGui::Text("Position:");
					vector3D position = this->scene_camera_transform.getPosition();
					if (ImGui::InputScalarN("##Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
					{
						this->scene_camera_transform.setPosition(position);
					};

					ImGui::Text("Rotation:");
					vector3D rotation = glm::degrees(glm::eulerAngles(this->scene_camera_transform.getOrientation()));
					if (ImGui::InputScalarN("##Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
					{
						this->scene_camera_transform.setOrientation(quaternionD(glm::radians(rotation)));
					}
					
					ImGui::Separator();
					const double MIN_MOVE_SPEED = 0.001;
					const double MAX_MOVE_SPEED = 10000.0;
					ImGui::Text("Linear Speed:");
					if (ImGui::DragScalar("##Linear_Speed", ImGuiDataType_::ImGuiDataType_Double, &this->linear_speed, 0.1, &MIN_MOVE_SPEED, &MAX_MOVE_SPEED, nullptr, 2.0f))
					{
						this->linear_speed = std::clamp(this->linear_speed, MIN_MOVE_SPEED, MAX_MOVE_SPEED);
					}

					const double MIN_ROTATION_SPEED = PI_D / 16;
					const double MAX_ROTATION_SPEED = PI_D / 4;
					ImGui::Text("Rotational Speed:");
					if (ImGui::DragScalar("##Rotation_Speed", ImGuiDataType_::ImGuiDataType_Double, &this->angular_speed, 0.01, &MIN_ROTATION_SPEED, &MAX_ROTATION_SPEED))
					{
						this->angular_speed = std::clamp(this->angular_speed, MIN_ROTATION_SPEED, MAX_ROTATION_SPEED);
					}

					ImGui::EndMenu();
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

		CameraStruct active_camera = {};
		active_camera.camera = this->scene_camera;
		active_camera.transform = this->scene_camera_transform;

		this->world_renderer->draw_scene(this->framebuffer_size, this->framebuffer, render_list, lighting, this->settings, active_camera);

		ImGui::Image((ImTextureID)this->legacy_backend->getFramebufferColorAttachment(this->framebuffer, 0), im_remaining_space, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		this->window_active = ImGui::IsWindowFocused();

		ImGui::End();
	}
}