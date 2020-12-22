#include "Sandbox/SandboxApplication.hpp"

//Hardcoded for now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"
#include "Genesis/Platform/FileSystem.hpp"

#include "imgui.h"

//Entity Components
#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"

//World Components
#include "Genesis/Physics/PhysicsWorld.hpp"

//Systems
#include "Genesis/System/TransformResolveSystem.hpp"
#include "Genesis/System/SceneSystem.hpp"

namespace Genesis
{
	SandboxApplication::SandboxApplication()
	{
		this->platform = new Genesis::SDL2_Platform(this);
		this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox Editor");

		this->legacy_backend = new Genesis::Opengl::OpenglBackend((Genesis::SDL2_Window*) window);
		this->ui_renderer = new Genesis::LegacyImGui(this->legacy_backend, this->input_manager, this->window);

		this->resource_manager = new ResourceManager(this->legacy_backend);

		this->sandbox_scene = new Scene();
		this->sandbox_scene->scene_components.add<SceneInfo>();

		this->world_renderer = new LegacySceneRenderer(this->legacy_backend);
	}

	SandboxApplication::~SandboxApplication()
	{
		delete this->sandbox_scene;
		delete this->ui_renderer;
		delete this->resource_manager;
		delete this->legacy_backend;
	}

	void SandboxApplication::update(Genesis::TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("SandboxApplication::update");
		Genesis::Application::update(time_step);

		TransformResolveSystem().run(this->sandbox_scene, time_step);

		if (this->sandbox_scene->scene_components.has<PhysicsWorld>())
		{
			auto& pre_view = this->sandbox_scene->registry.view<RigidBody, Transform>();
			for (EntityHandle entity : pre_view)
			{
				pre_view.get<RigidBody>(entity).setTransform(pre_view.get<Transform>(entity));
			}

			PhysicsWorld& physics = this->sandbox_scene->scene_components.get<PhysicsWorld>();
			physics.simulate(time_step);

			auto& post_view = this->sandbox_scene->registry.view<RigidBody, Transform>();
			for (EntityHandle entity : post_view)
			{
				post_view.get<RigidBody>(entity).getTransform(post_view.get<Transform>(entity));
			}
		}

	}

	void SandboxApplication::render(Genesis::TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("SandboxApplication::render");
		Application::render(time_step);

		SceneSystem::build_scene(this->sandbox_scene);

		this->legacy_backend->startFrame();
		Genesis::vector4F clear_color = Genesis::vector4F(0.0f, 0.0f, 0.0f, 1.0f);
		float clear_depth = 1.0f;
		this->legacy_backend->clearFramebuffer(true, true, &clear_color, &clear_depth);

		vector2U window_size = this->window->getWindowSize();
		ImVec2 image_size(this->framebuffer_size.x, this->framebuffer_size.y);

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

		this->world_renderer->drawScene(this->framebuffer_size, this->framebuffer, this->sandbox_scene->scene_components.get<SceneInfo>(), this->settings, active_camera);

		this->ui_renderer->beginFrame();

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(image_size);
		//ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);

		ImGui::Image((ImTextureID)this->legacy_backend->getFramebufferColorAttachment(this->framebuffer, 0), image_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		//ImGui::End();

		this->ui_renderer->endFrame();
		this->legacy_backend->endFrame();
	}
}