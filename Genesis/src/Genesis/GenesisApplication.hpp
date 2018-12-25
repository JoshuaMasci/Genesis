#pragma once

#include "Genesis/Core/DLL.hpp"
#include "Genesis/Platform/Window.hpp"

namespace Genesis
{
	class GENESIS_DLL GenesisApplication
	{
	public:
		GenesisApplication();
		virtual ~GenesisApplication();

		virtual void run_tick(double delta_time) = 0;

		bool isRunning();

	protected:
		Window* window = nullptr;
		bool is_running = true;
	};
};