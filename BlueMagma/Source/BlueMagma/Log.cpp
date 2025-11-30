#include "bmpch.hpp"
#include "Log.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace BM
{
	static inline struct LogData
	{
		struct Sink
		{
			spdlog::sink_ptr _Handle;
			bool _Core = true, _App = true;

			inline Sink() noexcept = default;
			inline Sink(const spdlog::sink_ptr& handle, bool core, bool app) noexcept
				: _Handle(handle), _Core(core), _App(app) {
			}
		};

		std::vector<Sink> _Sinks;

		std::shared_ptr<spdlog::logger> _CoreLogger;
		std::shared_ptr<spdlog::logger> _AppLogger;
	} s_Log;

	static inline spdlog::level::level_enum ConvertLevel(Log::Level level) noexcept {
		return static_cast<spdlog::level::level_enum>(level);
	}

	static inline void AddSink(const spdlog::sink_ptr& sink, bool core, bool app, const std::string& pattern) noexcept {
		sink->set_pattern(pattern);
		s_Log._Sinks.emplace_back(sink, core, app);
	}

	void Log::Init(const std::string& loggerName, Level flushOn, Level consoleLevel) noexcept
	{
		static bool alreadyInit = false;
		BM_CORE_ASSERT(!alreadyInit, "Log has already been initialized");

		if (consoleLevel != Off)
			AddConsoleSink(consoleLevel);

		auto& [sinks, coreLogger, appLogger] = s_Log;
		if (!alreadyInit)
		{
			using Sink = LogData::Sink;
			auto transform = std::views::transform(&Sink::_Handle);

			coreLogger = std::make_shared<spdlog::logger>("BlueMagma");
			coreLogger->sinks().assign_range(sinks | std::views::filter(&Sink::_Core) | transform);

			appLogger = std::make_shared<spdlog::logger>(loggerName);
			appLogger->sinks().assign_range(sinks | std::views::filter(&Sink::_App) | transform);

			spdlog::register_logger(coreLogger);
			spdlog::register_logger(appLogger);
		}
		alreadyInit = true;

		coreLogger->set_level(spdlog::level::trace);
		appLogger->set_level(spdlog::level::trace);

		coreLogger->flush_on(ConvertLevel(flushOn));
		appLogger->flush_on(ConvertLevel(flushOn));

		BM_CORE_FN("loggerName: {}, flushOn: {}, consoleLevel: {}", loggerName, (int)flushOn, (int)consoleLevel);
		BM_CORE_TRACE("CoreLogger Sinks: {}, AppLogger Sinks: {}", coreLogger->sinks().size(), appLogger->sinks().size());
	}

	void Log::AddConsoleSink(Level level, bool core, bool app, const std::string& pattern) noexcept
	{
		BM_CORE_ASSERT(core || app);

		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_level(ConvertLevel(level));

		AddSink(consoleSink, core, app, (pattern.empty() ? s_Pattern : pattern));
	}

	void Log::AddFileSink(const FileContext& context, bool core, bool app, const std::string& pattern) noexcept
	{
		BM_CORE_ASSERT(core || app);

		const auto& [name, level, rotateOnOpen, maxFiles, maxSize] = context;
		auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(name, maxSize, maxFiles, rotateOnOpen);
		fileSink->set_level(ConvertLevel(level));

		AddSink(fileSink, core, app, (pattern.empty() ? s_Pattern : pattern));
	}

	void Log::CoreLog(Level level, const std::string& message) noexcept
	{
		if (s_Log._CoreLogger)
			s_Log._CoreLogger->log(ConvertLevel(level), message);
	}

	void Log::AppLog(Level level, const std::string& message) noexcept
	{
		if (s_Log._AppLogger)
			s_Log._AppLogger->log(ConvertLevel(level), message);
	}
}
