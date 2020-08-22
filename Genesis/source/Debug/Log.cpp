#include "Genesis/Debug/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

//#define GENESIS_CONSOLE_LOG 1

namespace Genesis
{
	std::shared_ptr<spdlog::logger> Logging::Engine;
	std::shared_ptr<spdlog::logger> Logging::Application;
	std::shared_ptr<ConsoleWindowSinkMt> Logging::console_sink;

	void Logging::inti_engine_logging()
	{
		console_sink = std::make_shared<ConsoleWindowSinkMt>();
		console_sink->set_pattern("%^[%T] %n: %v%$");
		console_sink->set_level(spdlog::level::level_enum::trace);

#ifdef GENESIS_CONSOLE_LOG
		Engine = spdlog::stdout_color_mt("Genesis");
		Engine->set_level(spdlog::level::level_enum::trace);
		Engine->set_pattern("%^[%T] %n: %v%$");
#else
		Engine = std::make_shared<spdlog::logger>("Genesis", console_sink);
#endif
	}

	void Logging::inti_client_logging(std::string client_name)
	{
#ifdef GENESIS_CONSOLE_LOG
		Application = spdlog::stdout_color_mt(client_name);
		Application->set_level(spdlog::level::level_enum::trace);
		Application->set_pattern("%^[%T] %n: %v%$");
#else
		Application = std::make_shared<spdlog::logger>(client_name, console_sink);
#endif
	}
}