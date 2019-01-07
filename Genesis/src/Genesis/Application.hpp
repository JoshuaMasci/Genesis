#pragma once

#include "Genesis/Core/DLL.hpp"

#include "Genesis/Input/InputManager.hpp"
#include "Genesis/Event/EventSystem.hpp"

#include "Genesis/Platform/Platform.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	class GENESIS_DLL Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void run_tick(double delta_time);

		void close();
		bool isRunning();

		InputManager input_manager;
		EventSystem event_system;

		Platform* platform = nullptr;
		Window* window = nullptr;

	protected:
		bool is_running = true;
	};
};