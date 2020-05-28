#include "Genesis/Debug/Log.hpp"

using namespace Genesis;

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logging::Engine;
std::shared_ptr<spdlog::logger> Logging::Application;
std::shared_ptr<ConsoleWindowSinkMt> Logging::console_sink;

void Logging::inti_engine_logging(std::string engine_file_output)
{
	console_sink = std::make_shared<ConsoleWindowSinkMt>();
	console_sink->set_pattern("%^[%T] %n: %v%$");
	console_sink->set_level(spdlog::level::level_enum::trace);
	//Engine = std::make_shared<spdlog::logger>("Genesis", console_sink);

	Engine = spdlog::stdout_color_mt("Genesis");

	Engine->set_level(spdlog::level::level_enum::trace);
	Engine->set_pattern("%^[%T] %n: %v%$");
}

void Logging::inti_client_logging(std::string client_name, std::string client_file_output)
{
	//Application = std::make_shared<spdlog::logger>(client_name, console_sink);
	//Application->set_level(spdlog::level::level_enum::trace);

	Application = spdlog::stdout_color_mt(client_name);

	Application->set_level(spdlog::level::level_enum::trace);
	Application->set_pattern("%^[%T] %n: %v%$");
}
