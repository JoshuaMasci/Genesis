#pragma once

#include "Sandbox/SandboxApplication.hpp"

#include "Genesis/Core/Types.hpp"
#include "Genesis/Core/Log.hpp"
#include "Genesis/Core/Profiler.hpp"

int main(int argc, char** argv)
{
	GENESIS_PROFILE_START();

	Genesis::Logging::inti_engine_logging("");
	Genesis::Logging::inti_client_logging("Sandbox", "");
	GENESIS_INFO("Sandbox Started");

	GENESIS_PROFILE_BLOCK_START("Sandbox_Load");
	//ALPHA
	SandboxApplication* sandbox = new SandboxApplication();
	GENESIS_PROFILE_BLOCK_END();

	//Fixed Timestep Stuff
	//double time_step = 1.0 / 240.0;
	//double accumulator = 0.0;

	using clock = std::chrono::high_resolution_clock;
	auto time_last = clock::now();
	auto time_current = time_last;

	auto time_last_frame = time_last;
	size_t frames = 0;

	while (sandbox->isRunning())
	{
		GENESIS_PROFILE_BLOCK_START("Sandbox_Loop");

		time_current = clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_current - time_last);
		double delta_time = delta.count();

		//accumulator += delta_time;

		sandbox->runSimulation(delta_time);
		sandbox->drawWorld(delta_time);

		time_last = time_current;

		frames++;
		auto frame_delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_last - time_last_frame);
		if (frame_delta.count() > 1.0)
		{
			//printf("FPS: %zu\n", frames);
			sandbox->window->setWindowTitle("Sandbox: FPS " + std::to_string(frames));
			
			frames = 0;
			time_last_frame = time_last;
		}

		GENESIS_PROFILE_BLOCK_END();
	}

	GENESIS_PROFILE_BLOCK_START("Sandbox_Exit");
	delete sandbox;
	GENESIS_PROFILE_BLOCK_END();


	GENESIS_INFO("Sandbox Closed");

	GENESIS_PROFILE_WRITE_TO_FILE("Genesis_profile.prof");

	//Wait till enter
	getchar();

	return 0;
	//OMEGA
}