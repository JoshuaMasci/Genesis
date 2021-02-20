#include "genesis_engine/core/Application.hpp"

#include "genesis_engine/job_system/JobSystem.hpp"
#include "genesis_engine/input/InputManager.hpp"
#include "genesis_engine/platform/Platform.hpp"
#include "genesis_engine/platform/Window.hpp"

namespace genesis_engine 
{
	Application::Application()
	{
		//this->job_system = new JobSystem();
		this->input_manager = new InputManager("");
	}

	Application::~Application()
	{
		//Needs to wait till the GPU finishes before deleting stuff
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

		if (this->input_manager != nullptr)
		{
			delete this->input_manager;
		}

		if (this->job_system != nullptr)
		{
			delete this->job_system;
		}
	}

	void Application::run()
	{
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

			this->update(time_step);

			this->render(time_step);

			time_last = time_current;

			frames++;
			double frame_delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_last - time_last_frame).count();
			if (frame_delta > 1.0)
			{
				frames = 0;
				time_last_frame = time_last;
			}

			GENESIS_PROFILE_BLOCK_END();
		}

	}

	void Application::update(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("Application::update");

		if (this->input_manager != nullptr)
		{
			this->input_manager->update();
		}

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
}