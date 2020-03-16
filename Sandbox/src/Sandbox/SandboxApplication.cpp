#include "SandboxApplication.hpp"

//Hardcode For now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "VulkanBackend.hpp"

SandboxApplication::SandboxApplication()
{
	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox: ");
	this->rendering_backend = new Genesis::VulkanBackend(this->window, this->job_system.getNumberOfJobThreads());
	this->render_system = new Genesis::RenderSystem(this->rendering_backend);
	this->ui_renderer = new Genesis::ImGuiRenderer(this->rendering_backend, &this->input_manager);

	this->mesh_pool = new Genesis::MeshPool(this->rendering_backend);
	this->world_renderer = new Genesis::WorldRenderer(this->rendering_backend);

	this->world = new Genesis::World(this->mesh_pool);
}

SandboxApplication::~SandboxApplication()
{
	delete this->world;
	delete this->world_renderer;
	delete this->mesh_pool;
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

void SandboxApplication::render(Genesis::TimeStep interpolation_value)
{
	GENESIS_PROFILE_FUNCTION("SandboxApplication::render");

	if (this->startFrame())
	{
		Genesis::Application::render(interpolation_value);

		this->world_renderer->ImGuiDraw();
		this->world_renderer->drawWorld(interpolation_value, *this->world->world, this->world->camera);

		this->render_system->drawLayerWholeScreen(this->world_renderer);

		this->endFrame();
	}
}
