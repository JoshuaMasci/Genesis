#include "Sandbox/SandboxApplication.hpp"

//Hardcode For now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"

#include "Genesis/Rendering/Camera.hpp"

#include "imgui.h"


//Components
#include "Genesis/Component/NameComponent.hpp"

SandboxApplication::SandboxApplication()
{
	this->platform = new Genesis::SDL2_Platform(this);
	this->window = new Genesis::SDL2_Window(Genesis::vector2U(1600, 900), "Sandbox Editor");

	this->legacy_backend = new Genesis::Opengl::OpenglBackend((Genesis::SDL2_Window*) window);
	this->ui_renderer = new Genesis::LegacyImGui(this->legacy_backend, this->input_manager, this->window);
}

SandboxApplication::~SandboxApplication()
{
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
	Genesis::vector4F clear_color = Genesis::vector4F(0.0f, 0.0f, 0.0f, 1.0f);
	float clear_depth = 1.0f;
	this->legacy_backend->clearFramebuffer(true, true, &clear_color, &clear_depth);

	this->legacy_backend->endFrame();
}
