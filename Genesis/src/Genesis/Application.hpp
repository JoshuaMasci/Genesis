#pragma once

#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Input/InputManager.hpp"

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/Window.hpp"

#include "Genesis/GameScene.hpp"

namespace Genesis
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void runSimulation(double delta_time);
		virtual void drawFrame(double delta_time);

		void close();
		bool isRunning();

		//Engine Systems
		JobSystem job_system;
		InputManager input_manager;

		Platform* platform = nullptr;
		Window* window = nullptr;

		GameScene* scene = nullptr;

	protected:
		bool is_running = true;
	};
};