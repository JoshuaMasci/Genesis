#include "Sandbox/SandboxApplication.hpp"

//Hardcode For now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Ecs/DebugCamera.hpp"
#include "Genesis/Ecs/NameComponent.hpp"

SandboxApplication::SandboxApplication()
{
	Genesis::Logging::console_sink->setConsoleWindow(&this->console_window);

	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox: ");

	this->legacy_backend = new Genesis::Opengl::OpenglBackend((Genesis::SDL2_Window*) window);
	this->ui_renderer = new Genesis::LegacyImGui(this->legacy_backend, this->input_manager, this->window);

	this->world = new Genesis::World();

	/*this->ecs_world = new Genesis::EcsWorld();
	this->ecs_world->physics_world = new Genesis::PhysicsWorld(Genesis::vector3D(0.0));
	{
		this->ecs_world->main_camera = this->ecs_world->entity_registry.create();
		this->ecs_world->entity_registry.assign<Genesis::TransformD>(this->ecs_world->main_camera, Genesis::vector3D(0.0, 0.0, -20.0));
		this->ecs_world->entity_registry.assign<Genesis::Camera>(this->ecs_world->main_camera, 77.0f);
		this->ecs_world->entity_registry.assign<Genesis::DebugCamera>(this->ecs_world->main_camera, 5.0, 0.3);
		this->ecs_world->entity_registry.assign<Genesis::NameComponent>(this->ecs_world->main_camera, "Main_Camera");
	}

	//Systems
	{
		this->physics_system = new Genesis::PhyscisSystem();
		this->rendering_system = new Genesis::LegacyRenderingSystem(this->legacy_backend);
	}*/

	{
		this->offscreen_size = Genesis::vector2U(1024);
		Genesis::FramebufferAttachmentInfo color_attachment = { Genesis::TextureFormat::RGBA, Genesis::MultisampleCount::Sample_1 };
		Genesis::FramebufferDepthInfo depth_attachment = {Genesis::DepthFormat::depth_24,  Genesis::MultisampleCount::Sample_1 };
		Genesis::FramebufferCreateInfo create_info = {};
		create_info.attachments = &color_attachment;
		create_info.attachment_count = 1;
		create_info.depth_attachment = &depth_attachment;
		create_info.size = this->offscreen_size;
		this->offscreen_framebuffer = this->legacy_backend->createFramebuffer(create_info);
	}

	{
		Genesis::string file_name = "res/CesiumMan.gltf";

		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF loader;
		Genesis::string error;
		Genesis::string warning;
		bool return_value;

		if (file_name.substr(file_name.find_last_of(".") + 1) == "gltf")
		{
			return_value = loader.LoadASCIIFromFile(&gltfModel, &error, &warning, file_name);
		}
		else if (file_name.substr(file_name.find_last_of(".") + 1) == "glb")
		{
			return_value = loader.LoadBinaryFromFile(&gltfModel, &error, &warning, file_name);
		}
		else
		{
			GENESIS_ENGINE_ERROR("Unknown File extension");
		}

		if (!error.empty())
		{
			GENESIS_ENGINE_ERROR("Error: {}", error);
		}

		if (!warning.empty())
		{
			GENESIS_ENGINE_WARNING("Warning: {}", warning);
		}

		if (!return_value)
		{
			GENESIS_ENGINE_CRITICAL("Failed to parse glTF");
		}

		this->model = new Genesis::GltfModel(this->legacy_backend, gltfModel);

		/*Genesis::EntityHandle entity = this->ecs_world->entity_registry.create();
		this->ecs_world->entity_registry.assign<Genesis::TransformD>(entity);
		this->ecs_world->entity_registry.assign<Genesis::PbrMesh>(entity, this->model->meshes[0]);
		this->ecs_world->entity_registry.assign<Genesis::NameComponent>(entity, "Entity");*/
	}
}

SandboxApplication::~SandboxApplication()
{
	delete this->model;

	delete this->world;

	//delete this->rendering_system;
	//delete this->physics_system;

	//delete this->ecs_world->physics_world;
	//delete this->ecs_world;

	this->legacy_backend->destoryFramebuffer(this->offscreen_framebuffer);
	delete this->legacy_backend;

	Genesis::Logging::console_sink->setConsoleWindow(nullptr);
}

void SandboxApplication::update(Genesis::TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("SandboxApplication::update");
	Genesis::Application::update(time_step);

	this->world->runSimulation(this, time_step);
}

#include "imgui.h"

void SandboxApplication::render(Genesis::TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("SandboxApplication::render");

	Application::render(time_step);

	this->legacy_backend->startFrame();
	Genesis::vector4F clear_color = Genesis::vector4F(0.0f, 0.0f, 0.0f, 1.0f);
	float clear_depth = 1.0f;
	this->legacy_backend->clearFramebuffer(true, true, &clear_color, &clear_depth);

	this->ui_renderer->beginFrame();
	this->ui_renderer->beginDocking();
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("item 1", "")) { GENESIS_ENGINE_INFO("item 1"); };
			if (ImGui::MenuItem("item 2", "")) { GENESIS_ENGINE_INFO("item 2"); };
			if (ImGui::MenuItem("item 3", "")) { GENESIS_ENGINE_INFO("item 3"); };
			if (ImGui::MenuItem("item 4", "")) { GENESIS_ENGINE_INFO("item 4"); };
			if (ImGui::MenuItem("item 5", "")) { GENESIS_ENGINE_INFO("item 5"); };
			if (ImGui::MenuItem("item 6", "")) { GENESIS_ENGINE_INFO("item 6"); };
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	this->ui_renderer->endDocking();

	{
		Genesis::FrameStats stats = this->legacy_backend->getLastFrameStats();

		ImGui::Begin("Stats");
		ImGui::LabelText(std::to_string(time_step * 1000.0).c_str(), "Frame Time (ms)");
		ImGui::LabelText(std::to_string(stats.draw_calls).c_str(), "Draw Calls");
		ImGui::LabelText(std::to_string(stats.triangles_count).c_str(), "Tris count");
		
		ImGui::End();
	}

	{
		ImGui::Begin("GameView");
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 window_size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

		//Rebuild Framebuffer
		Genesis::vector2U new_framebuffer_size = Genesis::vector2U((uint32_t)window_size.x, (uint32_t)window_size.y);
		if (new_framebuffer_size != this->offscreen_size)
		{
			this->legacy_backend->destoryFramebuffer(this->offscreen_framebuffer);

			this->offscreen_size = new_framebuffer_size;

			Genesis::FramebufferAttachmentInfo color_attachment = { Genesis::TextureFormat::RGBA, Genesis::MultisampleCount::Sample_1 };
			Genesis::FramebufferDepthInfo depth_attachment = { Genesis::DepthFormat::depth_24,  Genesis::MultisampleCount::Sample_1 };
			Genesis::FramebufferCreateInfo create_info = {};
			create_info.attachments = &color_attachment;
			create_info.attachment_count = 1;
			create_info.depth_attachment = &depth_attachment;
			create_info.size = this->offscreen_size;
			this->offscreen_framebuffer = this->legacy_backend->createFramebuffer(create_info);
		}

		this->legacy_backend->bindFramebuffer(this->offscreen_framebuffer);
		this->legacy_backend->clearFramebuffer(true, true);

		//TODO render here
		//this->rendering_system->render(this->offscreen_size, this->ecs_world, time_step);

		this->legacy_backend->bindFramebuffer(nullptr);

		ImGui::Image((ImTextureID)this->legacy_backend->getFramebufferColorAttachment(this->offscreen_framebuffer, 0), window_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::End();
	}

	this->console_window.drawWindow();
	this->world_view_window.drawWindow(this->world);
	//this->entity_properties_window.drawWindow(this->ecs_world, this->world_view_window.selected_entity);

	this->ui_renderer->endFrame();

	this->legacy_backend->endFrame();
}
