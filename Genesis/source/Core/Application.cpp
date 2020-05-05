#include "Genesis/Core/Application.hpp"

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
	/*const double SimulationStep = 1.0 / (60.0 * 3);
	const TimeStep MaxTimeStep = 1.0 / 60.0;
	double accumulator = SimulationStep;*/


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
		TimeStep time_step = (TimeStep)std::chrono::duration_cast<std::chrono::duration<double>>(time_current - time_last).count();
		/*if (time_step > MaxTimeStep)
		{
			time_step = MaxTimeStep;
		}

		accumulator += time_step;

		while (accumulator >= SimulationStep)
		{
			GENESIS_PROFILE_BLOCK_START("Application_SimulationStep");
			this->update(SimulationStep);
			accumulator -= SimulationStep;
			GENESIS_PROFILE_BLOCK_END();
		}

		TimeStep interpolation_value = accumulator / SimulationStep;*/

		this->update(time_step);

		this->render(time_step);

		time_last = time_current;

		frames++;
		double frame_delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_last - time_last_frame).count();
		if (frame_delta > 1.0)
		{
			this->window->setWindowTitle("Sandbox: FPS " + std::to_string(frames));

			frames = 0;
			time_last_frame = time_last;
		}

		GENESIS_PROFILE_BLOCK_END();
	}

}

#include "Genesis/Entity/DebugCamera.hpp"
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
}

void Application::close()
{
	this->is_running = false;
}

bool Application::isRunning()
{
	return this->is_running;
}