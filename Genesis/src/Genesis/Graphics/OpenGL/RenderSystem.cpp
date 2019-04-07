#include "RenderSystem.hpp"

using namespace Genesis;

RenderSystem::RenderSystem(Window* window)
{
	this->window = window;
}

RenderSystem::~RenderSystem()
{

}

void RenderSystem::drawFrame(EntityRegistry& registry, JobSystem& job_system, double delta_time)
{

}
