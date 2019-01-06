#pragma once

#include "Genesis/Core/DLL.hpp"
#include "Genesis/Platform/Window.hpp"
#include "Genesis/Event/EventSystem.hpp"

namespace Genesis
{
	class GENESIS_DLL Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void run_tick(double delta_time) = 0;

		bool isRunning();

		EventSystem event_system;

	protected:
		Window* window = nullptr;

		bool is_running = true;
	};
};