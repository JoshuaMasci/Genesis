#include "Application.hpp"

#include "Genesis/Debug/Profiler.hpp"

using namespace Genesis;

Application::Application()
	:input_manager("config/input")
{

}

Application::~Application()
{
	//Needs to wait till the GPU finishes before deleting stuff
	if (this->rendering_backend != nullptr)
	{
		this->rendering_backend->waitTillDone();
	}

	if (this->render_system != nullptr)
	{
		delete this->render_system;
	}

	if (this->rendering_backend != nullptr)
	{
		delete this->rendering_backend;
	}

	if (this->window != nullptr)
	{
		delete this->window;
	}

	if (this->platform != nullptr)
	{
		delete this->platform;
	}
}

void Application::run()
{
	//Fixed Timestep Stuff
	//double time_step = 1.0 / 240.0;
	//double accumulator = 0.0;

	//Mode: Rendering and Simulation Linked
	using clock = std::chrono::high_resolution_clock;
	auto time_last = clock::now();
	auto time_current = time_last;

	auto time_last_frame = time_last;
	size_t frames = 0;

	while (this->isRunning())
	{
		GENESIS_PROFILE_BLOCK_START("Application_Loop");

		time_current = clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_current - time_last);
		TimeStep time_step = (TimeStep)delta.count();

		this->update(time_step);
		this->render(time_step);

		time_last = time_current;

		frames++;
		auto frame_delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_last - time_last_frame);
		if (frame_delta.count() > 1.0)
		{
			this->window->setWindowTitle("Sandbox: FPS " + std::to_string(frames));

			frames = 0;
			time_last_frame = time_last;
		}

		GENESIS_PROFILE_BLOCK_END();
	}

}

void Application::update(TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("Application::update");
	this->input_manager.update();

	if (this->platform != nullptr)
	{
		this->platform->onUpdate(time_step);
	}

}

void Application::render(TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("Application::render");

	if (this->render_system->startFrame())
	{
		this->render_system->endFrame();
	}
}

void Application::close()
{
	this->is_running = false;
}

bool Application::isRunning()
{
	return this->is_running;
}