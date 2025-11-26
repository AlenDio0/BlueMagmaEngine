#include "bmpch.hpp"
#include "Log.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace BM
{
	namespace Log
	{
		static inline struct LogData
		{
			std::shared_ptr<spdlog::logger> _CoreLogger;
			std::shared_ptr<spdlog::logger> _AppLogger;

			inline LogData() noexcept {
				SetPattern("%^[%T] <%n> %v.%$");

				_CoreLogger = spdlog::stdout_color_mt("BlueMagma");
				_AppLogger = spdlog::stdout_color_mt("App");

				SetLevel(Level::Trace);
			}
		} s_Log;

		void SetLevel(Level level) noexcept
		{
			s_Log._CoreLogger->set_level(static_cast<spdlog::level::level_enum>(level));
			s_Log._AppLogger->set_level(static_cast<spdlog::level::level_enum>(level));
		}

		void SetPattern(const std::string& pattern) noexcept
		{
			spdlog::set_pattern(pattern);
		}

		void CoreLog(Level level, const std::string& message) noexcept
		{
			s_Log._CoreLogger->log((spdlog::level::level_enum)level, message);
		}

		void AppLog(Level level, const std::string& message) noexcept
		{
			s_Log._AppLogger->log((spdlog::level::level_enum)level, message);
		}
	}
}
