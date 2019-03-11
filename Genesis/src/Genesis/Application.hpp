#pragma once

#include "Genesis/Job/JobSystem.hpp"
#include "Genesis/Input/InputManager.hpp"

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/Window.hpp"

#include <entt/entity/registry.hpp>

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

		//Engine Systems
		JobSystem job_system;
		InputManager input_manager;

		Platform* platform = nullptr;
		Window* window = nullptr;

		//EnTT
		entt::registry<> entity_registry;

	protected:
		bool is_running = true;
	};
};