#include "RenderSystem.hpp"

#include "Genesis/Graphics/OpenGL/OpenGL.hpp"
#include "Genesis/Graphics/OpenGL/TexturedMesh.hpp"

using namespace Genesis;

RenderSystem::RenderSystem(Window* window)
{
	this->window = window;
	this->context = this->window->GL_CreateContext();

	this->window->GL_SetVsync(Vsync::ON);
}

RenderSystem::~RenderSystem()
{
	this->window->GL_DeleteContext(this->context);
}

void RenderSystem::drawFrame(EntityRegistry& registry, JobSystem& job_system, double delta_time)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->window->GL_UpdateBuffer();
}
