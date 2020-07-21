#include "Genesis_Editor/Windows/SceneViewWindow.hpp"

#include "imgui.h"

namespace Genesis
{
	SceneViewWindow::SceneViewWindow(InputManager* input_manager, LegacyBackend* legacy_backend)
	{
		this->input_manager = input_manager;

		this->legacy_backend = legacy_backend;
		this->world_renderer = new LegacyWorldRenderer(this->legacy_backend);

	}

	SceneViewWindow::~SceneViewWindow()
	{
		delete this->world_renderer;
		if (this->framebuffer != nullptr)
		{
			this->legacy_backend->destoryFramebuffer(this->framebuffer);
			this->legacy_backend->destoryFramebuffer(this->gamma_correct_framebuffer);
		}
	}

	void SceneViewWindow::udpate(TimeStep time_step)
	{
		if (this->is_window_active)
		{
			vector3D position = this->scene_camera_transform.getPosition();
			position += (this->scene_camera_transform.getForward() * input_manager->getButtonAxisCombo("Debug_ForwardBackward", "Debug_Forward", "Debug_Backward")) * this->linear_speed * time_step;
			position += (this->scene_camera_transform.getUp() * input_manager->getButtonAxisCombo("Debug_UpDown", "Debug_Up", "Debug_Down")) * this->linear_speed * time_step;
			position += (this->scene_camera_transform.getLeft() * input_manager->getButtonAxisCombo("Debug_LeftRight", "Debug_Left", "Debug_Right")) * this->linear_speed * time_step;
			this->scene_camera_transform.setPosition(position);

			quaternionD orientation = this->scene_camera_transform.getOrientation();
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Pitch", "Debug_PitchUp", "Debug_PitchDown", false) * this->angular_speed * (PI_D * 2.0) * time_step, this->scene_camera_transform.getLeft()) * orientation;
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Yaw", "Debug_YawLeft", "Debug_YawRight", false) * this->angular_speed * (PI_D * 2.0) * time_step, this->scene_camera_transform.getUp()) * orientation;
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Roll", "Debug_RollRight", "Debug_RollLeft", false) * this->angular_speed * (PI_D)* time_step, this->scene_camera_transform.getForward()) * orientation;
			this->scene_camera_transform.setOrientation(orientation);
		}
	}

	void SceneViewWindow::drawWindow(EntityWorld* world)
	{
		ImGui::Begin("Scene View");

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 im_window_size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);
		vector2U window_size = vector2U(vMax.x - vMin.x, vMax.y - vMin.y);

		if (window_size != this->framebuffer_size)
		{
			//Rebuild Framebuffer
		
			if (this->framebuffer != nullptr)
			{
				this->legacy_backend->destoryFramebuffer(this->framebuffer);
			}

			this->framebuffer_size = window_size;
			FramebufferAttachmentInfo color_attachment = { TextureFormat::RGBA, MultisampleCount::Sample_1 };
			FramebufferDepthInfo depth_attachment = { DepthFormat::depth_24,  MultisampleCount::Sample_1 };
			FramebufferCreateInfo create_info = {};
			create_info.attachments = &color_attachment;
			create_info.attachment_count = 1;
			create_info.depth_attachment = &depth_attachment;
			create_info.size = this->framebuffer_size;

			this->framebuffer = this->legacy_backend->createFramebuffer(create_info);

			//Gamma Corrected Framebuffer doesn't need depth attachment
			//create_info.depth_attachment = nullptr;
			//this->gamma_correct_framebuffer = this->legacy_backend->createFramebuffer(create_info);
		}

		this->world_renderer->drawScene(this->framebuffer_size, this->framebuffer, world, this->scene_camera, this->scene_camera_transform);

		ImGui::Image((ImTextureID)this->legacy_backend->getFramebufferColorAttachment(this->framebuffer, 0), im_window_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		this->is_window_active = ImGui::IsWindowFocused();

		ImGui::End();
	}
}