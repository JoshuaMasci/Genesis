#pragma once

#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Input/InputManager.hpp"
#include "Genesis/Event/EventSystem.hpp"

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/Window.hpp"

#include "Genesis/World/World.hpp"
#include "Genesis/World/EntityManager.hpp"

namespace Genesis
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void runFrame(double delta_time);

		void close();
		bool isRunning();

		//World
		EntityManager entity_manager;
		World* world = nullptr;

		//Engine Systems
		JobSystem job_system;
		InputManager input_manager;
		EventSystem event_system;

		Platform* platform = nullptr;
		Window* window = nullptr;

	protected:
		bool is_running = true;
	};
};