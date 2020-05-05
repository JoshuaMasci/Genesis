#pragma once

#include <memory>

#define FMT_HEADER_ONLY
#define FMT_DEPRECATED
#include <spdlog/spdlog.h>

#include "Genesis/Editor/ConsoleWindowSink.hpp"

namespace Genesis
{
	class Logging
	{
	public:
		static void inti_engine_logging(std::string engine_file_output);
		static void inti_client_logging(std::string client_name, std::string client_file_output);

		static std::shared_ptr<spdlog::logger> Engine;
		static std::shared_ptr<spdlog::logger> Application;
		static std::shared_ptr<ConsoleWindowSinkMt> console_sink;
	};
}

#define GENESIS_LOGGING_ENABLED
#ifdef GENESIS_LOGGING_ENABLED


#define GENESIS_ENGINE_TRACE(...) Genesis::Logging::Engine->trace(__VA_ARGS__)
#define GENESIS_ENGINE_DEBUG(...) Genesis::Logging::Engine->debug(__VA_ARGS__)
#define GENESIS_ENGINE_INFO(...) Genesis::Logging::Engine->info(__VA_ARGS__)
#define GENESIS_ENGINE_WARNING(...) Genesis::Logging::Engine->warn(__VA_ARGS__)
#define GENESIS_ENGINE_ERROR(...) Genesis::Logging::Engine->error(__VA_ARGS__)
#define GENESIS_ENGINE_CRITICAL(...) Genesis::Logging::Engine->critical(__VA_ARGS__)

#define GENESIS_TRACE(...) Genesis::Logging::Application->trace(__VA_ARGS__)
#define GENESIS_DEBUG(...) Genesis::Logging::Application->debug(__VA_ARGS__)
#define GENESIS_INFO(...) Genesis::Logging::Application->info(__VA_ARGS__)
#define GENESIS_WARN(...) Genesis::Logging::Application->warn(__VA_ARGS__)
#define GENESIS_ERROR(...) Genesis::Logging::Application->error(__VA_ARGS__)
#define GENESIS_CRITICAL(...) Genesis::Logging::Application->critical(__VA_ARGS__)

#else

#define GENESIS_ENGINE_TRACE(...)
#define GENESIS_ENGINE_DEBUG(...)
#define GENESIS_ENGINE_INFO(...)
#define GENESIS_ENGINE_WARN(...)
#define GENESIS_ENGINE_ERROR(...)
#define GENESIS_ENGINE_CRITICAL(...)

#define GENESIS_TRACE(...)
#define GENESIS_DEBUG(...)
#define GENESIS_INFO(...)
#define GENESIS_WARN(...)
#define GENESIS_ERROR(...)
#define GENESIS_CRITICAL(...)

#endif // USE_LOGGING
