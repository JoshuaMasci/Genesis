#pragma once

#include <memory>

#define FMT_HEADER_ONLY
#define FMT_DEPRECATED
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Genesis
{
	class Logging
	{
	public:
		static void inti_engine_logging(std::string engine_file_output);
		static void inti_client_logging(std::string client_name, std::string client_file_output);

		inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return engine_logger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return client_logger; }

	private:
		static std::shared_ptr<spdlog::logger> engine_logger;
		static std::shared_ptr<spdlog::logger> client_logger;
	};
}

#define GENESIS_ENGINE_TRACE(...) Genesis::Logging::GetEngineLogger()->trace(__VA_ARGS__)
#define GENESIS_ENGINE_DEBUG(...) Genesis::Logging::GetEngineLogger()->debug(__VA_ARGS__)
#define GENESIS_ENGINE_INFO(...) Genesis::Logging::GetEngineLogger()->info(__VA_ARGS__)
#define GENESIS_ENGINE_WARN(...) Genesis::Logging::GetEngineLogger()->warn(__VA_ARGS__)
#define GENESIS_ENGINE_ERROR(...) Genesis::Logging::GetEngineLogger()->error(__VA_ARGS__)
#define GENESIS_ENGINE_CRITICAL(...) Genesis::Logging::GetEngineLogger()->critical(__VA_ARGS__)

#define GENESIS_TRACE(...) Genesis::Logging::GetClientLogger()->trace(__VA_ARGS__)
#define GENESIS_DEBUG(...) Genesis::Logging::GetClientLogger()->debug(__VA_ARGS__)
#define GENESIS_INFO(...) Genesis::Logging::GetClientLogger()->info(__VA_ARGS__)
#define GENESIS_WARN(...) Genesis::Logging::GetClientLogger()->warn(__VA_ARGS__)
#define GENESIS_ERROR(...) Genesis::Logging::GetClientLogger()->error(__VA_ARGS__)
#define GENESIS_CRITICAL(...) Genesis::Logging::GetClientLogger()->critical(__VA_ARGS__)