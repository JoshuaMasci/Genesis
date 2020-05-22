#include "Sandbox/SandboxApplication.hpp"

//Hardcode For now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyWorldRenderer.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"

SandboxApplication::SandboxApplication()
{
	Genesis::Logging::console_sink->setConsoleWindow(&this->console_window);

	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox: ");

	this->legacy_backend = new Genesis::Opengl::OpenglBackend((Genesis::SDL2_Window*) window);
	this->world_renderer = new Genesis::LegacyWorldRenderer(this->legacy_backend);
	this->ui_renderer = new Genesis::LegacyImGui(this->legacy_backend, this->input_manager, this->window);

	this->world = new Genesis::World(this->world_renderer);

	{
		this->offscreen_size = Genesis::vector2U(2048, 2048);
		Genesis::FramebufferAttachmentInfo color_attachment = { Genesis::TextureFormat::RGBA, Genesis::MultisampleCount::Sample_1 };
		Genesis::FramebufferDepthInfo depth_attachment = {Genesis::DepthFormat::depth_24,  Genesis::MultisampleCount::Sample_1 };
		Genesis::FramebufferCreateInfo create_info = {};
		create_info.attachments = &color_attachment;
		create_info.attachment_count = 1;
		create_info.depth_attachment = &depth_attachment;
		create_info.size = this->offscreen_size;
		this->offscreen_framebuffer = this->legacy_backend->createFramebuffer(create_info);
	}
}

SandboxApplication::~SandboxApplication()
{
	this->legacy_backend->destoryFramebuffer(this->offscreen_framebuffer);

	delete this->world;
	delete this->world_renderer;
	delete this->legacy_backend;

	Genesis::Logging::console_sink->setConsoleWindow(nullptr);
}

void SandboxApplication::update(Genesis::TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("SandboxApplication::update");
	Genesis::Application::update(time_step);

	if (this->world != nullptr)
	{
		this->world->runSimulation(this, time_step);
	}
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
		ImVec2 size = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

		this->legacy_backend->bindFramebuffer(this->offscreen_framebuffer);
		this->legacy_backend->clearFramebuffer(true, true);
		this->world_renderer->drawWorld(this->world, Genesis::vector2U(size.x, size.y));
		this->legacy_backend->bindFramebuffer(nullptr);

		ImGui::Image((void*)(intptr_t)this->legacy_backend->getFramebufferColorAttachment(this->offscreen_framebuffer, 0), size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::End();
	}

	this->console_window.drawWindow("Console");
	this->world_view_window.drawWindow(this->world);
	this->entity_properties_window.drawWindow(this->world, this->world_view_window.getSelectedEntity());

	this->ui_renderer->endFrame();

	this->legacy_backend->endFrame();
}