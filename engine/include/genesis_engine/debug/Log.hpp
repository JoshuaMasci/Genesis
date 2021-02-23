#pragma once

#include <memory>

#define FMT_HEADER_ONLY
#define FMT_DEPRECATED
#include <spdlog/spdlog.h>

#include "genesis_engine/editor/console_window_sink.hpp"

namespace genesis
{
	class Logging
	{
	public:
		static void inti_engine_logging();
		static void inti_client_logging(std::string client_name);

		static std::shared_ptr<spdlog::logger> Engine;
		static std::shared_ptr<spdlog::logger> Application;
		static std::shared_ptr<ConsoleWindowSinkMt> console_sink;
	};
}

#define GENESIS_LOGGING_ENABLED
#ifdef GENESIS_LOGGING_ENABLED


#define GENESIS_ENGINE_TRACE(...) genesis::Logging::Engine->trace(__VA_ARGS__)
#define GENESIS_ENGINE_DEBUG(...) genesis::Logging::Engine->debug(__VA_ARGS__)
#define GENESIS_ENGINE_INFO(...) genesis::Logging::Engine->info(__VA_ARGS__)
#define GENESIS_ENGINE_WARNING(...) genesis::Logging::Engine->warn(__VA_ARGS__)
#define GENESIS_ENGINE_ERROR(...) genesis::Logging::Engine->error(__VA_ARGS__)
#define GENESIS_ENGINE_CRITICAL(...) genesis::Logging::Engine->critical(__VA_ARGS__)

#define GENESIS_TRACE(...) genesis::Logging::Application->trace(__VA_ARGS__)
#define GENESIS_DEBUG(...) genesis::Logging::Application->debug(__VA_ARGS__)
#define GENESIS_INFO(...) genesis::Logging::Application->info(__VA_ARGS__)
#define GENESIS_WARNING(...) genesis::Logging::Application->warn(__VA_ARGS__)
#define GENESIS_ERROR(...) genesis::Logging::Application->error(__VA_ARGS__)
#define GENESIS_CRITICAL(...) genesis::Logging::Application->critical(__VA_ARGS__)

#else

#define GENESIS_ENGINE_TRACE(...)
#define GENESIS_ENGINE_DEBUG(...)
#define GENESIS_ENGINE_INFO(...)
#define GENESIS_ENGINE_WARNING(...)
#define GENESIS_ENGINE_ERROR(...)
#define GENESIS_ENGINE_CRITICAL(...)

#define GENESIS_TRACE(...)
#define GENESIS_DEBUG(...)
#define GENESIS_INFO(...)
#define GENESIS_WARNING(...)
#define GENESIS_ERROR(...)
#define GENESIS_CRITICAL(...)

#endif // USE_LOGGING
