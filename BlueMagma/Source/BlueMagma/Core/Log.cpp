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
			spdlog::sink_ptr _CoreHandle;
			spdlog::sink_ptr _AppHandle;
		};
		std::vector<Sink> _Sinks;

		std::shared_ptr<spdlog::logger> _CoreLogger;
		std::shared_ptr<spdlog::logger> _AppLogger;
	} s_Log;

	static inline spdlog::level::level_enum ConvertLevel(Log::Level level) noexcept {
		return static_cast<spdlog::level::level_enum>(level);
	}

	template<typename TSink, typename... Args>
	static inline spdlog::sink_ptr CreateSink(Log::Level level, const std::string& pattern, Args&&... args) noexcept {
		if (level == Log::Off)
			return nullptr;

		spdlog::sink_ptr sink = std::make_shared<TSink>(std::forward<Args>(args)...);
		sink->set_level(ConvertLevel(level));
		sink->set_pattern(pattern);

		return sink;
	}

	template<typename TSink, typename... Args>
	static inline void AddSink(Log::Level core, Log::Level app, const std::string& pattern, Args&&... args) noexcept {
		s_Log._Sinks.emplace_back(
			CreateSink<TSink>(core, pattern, std::forward<Args>(args)...),
			CreateSink<TSink>(app, pattern, std::forward<Args>(args)...)
		);
	}

	void Log::Init(const std::string& loggerName, Level flushOn, Level consoleLevel) noexcept
	{
		static bool init = false;
		BM_CORE_ASSERT(!init, "Log has already been initialized");

		if (consoleLevel != Off)
			AddConsoleSink(consoleLevel);

		auto& [sinks, coreLogger, appLogger] = s_Log;
		if (!init)
		{
			using Sink = LogData::Sink;

			coreLogger = std::make_shared<spdlog::logger>("BlueMagma");
			appLogger = std::make_shared<spdlog::logger>(loggerName);

			auto coreSinks = sinks | std::views::filter([](const Sink& x) { return x._CoreHandle != nullptr; }) |
				std::views::transform(&Sink::_CoreHandle);
			coreLogger->sinks().assign(coreSinks.begin(), coreSinks.end());

			auto appSinks = sinks | std::views::filter([](const Sink& x) { return x._AppHandle != nullptr; }) |
				std::views::transform(&Sink::_AppHandle);
			appLogger->sinks().assign(appSinks.begin(), appSinks.end());

			spdlog::register_logger(coreLogger);
			spdlog::register_logger(appLogger);

			init = true;
		}

		coreLogger->set_level(spdlog::level::trace);
		appLogger->set_level(spdlog::level::trace);

		coreLogger->flush_on(ConvertLevel(flushOn));
		appLogger->flush_on(ConvertLevel(flushOn));

		BM_CORE_FN("loggerName: {}, flushOn: {}, consoleLevel: {}", loggerName, (int)flushOn, (int)consoleLevel);
		BM_CORE_TRACE("CoreLogger Sinks: {}, AppLogger Sinks: {}", coreLogger->sinks().size(), appLogger->sinks().size());

		coreLogger->flush();
		appLogger->flush();
	}

	void Log::AddConsoleSink(Level core, Level app, const std::string& pattern) noexcept
	{
		AddSink<spdlog::sinks::stdout_color_sink_mt>(core, app, (pattern.empty() ? s_Pattern : pattern));
	}

	void Log::AddFileSink(const FileContext& context, Level core, Level app, const std::string& pattern) noexcept
	{
		const auto& [name, maxFiles, maxSize, rotateOnOpen] = context;
		AddSink<spdlog::sinks::rotating_file_sink_mt>(core, app, (pattern.empty() ? s_Pattern : pattern), name, maxSize, maxFiles, rotateOnOpen);
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
