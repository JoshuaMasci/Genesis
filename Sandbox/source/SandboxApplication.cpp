#include "Sandbox/SandboxApplication.hpp"

//Hardcoded for now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"
#include "Genesis/Platform/FileSystem.hpp"

#include "imgui.h"

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
	}

	void SandboxApplication::render(Genesis::TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("SandboxApplication::render");
		Application::render(time_step);

		this->legacy_backend->startFrame();
		this->legacy_backend->endFrame();
	}
}