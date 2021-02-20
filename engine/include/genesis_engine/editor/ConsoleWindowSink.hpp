#pragma once

#include "spdlog/sinks/base_sink.h"
#include "genesis_engine/editor/ConsoleWindow.hpp"

namespace Genesis
{
	template<typename Mutex>
	class ConsoleWindowSink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		void setConsoleWindow(ConsoleWindow* window) { this->console = window; };

	protected:
		ConsoleLevel getLevel(spdlog::level::level_enum level)
		{
			switch (level)
			{
			case spdlog::level::trace:
				return ConsoleLevel::trace;
			case spdlog::level::debug:
				return ConsoleLevel::debug;
			case spdlog::level::info:
				return ConsoleLevel::info;
			case spdlog::level::warn:
				return ConsoleLevel::warning;
			case spdlog::level::err:
				return ConsoleLevel::error;
			case spdlog::level::critical:
				return ConsoleLevel::critical;
			case spdlog::level::off:
				return ConsoleLevel::off;
			}
			return ConsoleLevel::off;
		};

		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			if (this->console != nullptr)
			{
				spdlog::memory_buf_t formatted;
				base_sink<Mutex>::formatter_->format(msg, formatted);

				ConsoleMessage message = { getLevel(msg.level), fmt::to_string(formatted), msg.thread_id };
				console->addMessage(message);
			}
		};

		void flush_() override {};

		ConsoleWindow* console = nullptr;
	};

#include "spdlog/details/null_mutex.h"
#include <mutex>
	using ConsoleWindowSinkSt = ConsoleWindowSink<spdlog::details::null_mutex>;
	using ConsoleWindowSinkMt = ConsoleWindowSink<std::mutex>;
}