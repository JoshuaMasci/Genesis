#include "Log.hpp"

using namespace Genesis;

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logging::engine_logger;
std::shared_ptr<spdlog::logger> Logging::client_logger;

void Logging::inti_engine_logging(std::string engine_file_output)
{
	spdlog::set_level(spdlog::level::debug);
	spdlog::set_pattern("%^[%T] %n: %v%$");
	//engine_logger = spdlog::basic_logger_mt("Genesis", engine_file_output);
	engine_logger = spdlog::stdout_color_mt("Genesis");
}

void Logging::inti_client_logging(std::string client_name, std::string client_file_output)
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	client_logger = spdlog::stdout_color_mt(client_name);
}
