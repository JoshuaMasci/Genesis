#pragma once

#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Input/InputManager.hpp"

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/Window.hpp"
#include <Genesis/Rendering/RenderingBackend.hpp>

#include "Genesis/GameScene.hpp"

//#define TBB_SUPPRESS_DEPRECATED_MESSAGES 1
//#include <tbb/tbb.h>

namespace Genesis
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void runSimulation(double delta_time);
		virtual void drawWorld(double delta_time);

		void close();
		bool isRunning();

		//Engine Systems
		JobSystem job_system;
		//tbb::task_scheduler_init* scheduler = nullptr;

		InputManager input_manager;

		Platform* platform = nullptr;
		Window* window = nullptr;
		RenderingBackend* rendering_backend = nullptr;

		GameScene* scene = nullptr;

	protected:
		bool is_running = true;
	};
};