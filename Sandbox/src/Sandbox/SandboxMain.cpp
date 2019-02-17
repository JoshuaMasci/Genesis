#pragma once

#include "Sandbox/SandboxApplication.hpp"

int main(int argc, char** argv)
{
	//ALPHA
	SandboxApplication* sandbox = new SandboxApplication();

	using clock = std::chrono::high_resolution_clock;
	auto time_last = clock::now();
	auto time_current = time_last;

	auto time_last_frame = time_last;
	size_t frames = 0;

	while (sandbox->isRunning())
	{
		time_current = clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_current - time_last);
		double delta_time = delta.count();

		sandbox->runFrame(delta_time);

		time_last = time_current;

		frames++;
		auto frame_delta = std::chrono::duration_cast<std::chrono::duration<double>>(time_last - time_last_frame);
		if (frame_delta.count() > 1.0)
		{
			printf("FPS: %zu\n", frames);
			frames = 0;
			time_last_frame = time_last;
		}
	}

	delete sandbox;

	return 0;
	//OMEGA
}