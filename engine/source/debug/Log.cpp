#include "genesis_engine/debug/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

//#define GENESIS_CONSOLE_LOG 1

namespace Genesis
{
	std::shared_ptr<spdlog::logger> Logging::Engine;
	std::shared_ptr<spdlog::logger> Logging::Application;
	std::shared_ptr<ConsoleWindowSinkMt> Logging::console_sink;

	void Logging::inti_engine_logging()
	{
		Engine = spdlog::stdout_color_mt("Genesis");
		Engine->set_level(spdlog::level::level_enum::trace);
		Engine->set_pattern("%^[%T] %n: %v%$");
	}

	void Logging::inti_client_logging(std::string client_name)
	{
		console_sink = std::make_shared<ConsoleWindowSinkMt>();
		Application = std::make_shared<spdlog::logger>(client_name, console_sink);
		Application->set_pattern("%^[%T] %n: %v%$");
		Application->set_level(spdlog::level::level_enum::trace);
	}
}