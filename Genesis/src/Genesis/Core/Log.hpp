/* Currently not working, need to fix later
#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Genesis
{
	//Only a class to privide static variables
	class Logging
	{
	public:
		static void inti_engine_logging(std::string engine_file_output)
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			engine_logger = spdlog::basic_logger_mt("Genesis", engine_file_output);
		}

		static void inti_client_logging(std::string client_name, std::string client_file_output)
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			client_logger = spdlog::basic_logger_mt(client_name, client_file_output);
		}

		static std::shared_ptr<spdlog::logger> engine_logger;
		//static std::shared_ptr<spdlog::logger> client_logger;

	private:
		//No instantiation
		Logging() {};
	};
};

#define GENESIS_ENGINE_INFO(...) ::Genesis::Logging::engine_logger->info(__VA_ARGS__)
#define GENESIS_ENGINE_WARN(...) ::Genesis::Logging::engine_logger->warn(__VA_ARGS__)
#define GENESIS_ENGINE_ERROR(...) ::Genesis::Logging::engine_logger->error(__VA_ARGS__)

//#define GENESIS_INFO(...) ::Genesis::Logging::client_logger->info(__VA_ARGS__)
//#define GENESIS_WARN(...) ::Genesis::Logging::client_logger->warn(__VA_ARGS__)
//#define GENESIS_ERROR(...) ::Genesis::Logging::client_logger->error(__VA_ARGS__)
*/