#pragma once
#include <string>
#include <format>

#if _DEBUG || true
#define BM_ENABLE_LOG
#endif

// Converts Megabyte in Bytes
#define BM_MB(x) x * 1024 * 1024

namespace BM
{
	class Log
	{
	public:
		enum Level
		{
			Trace = 0,
			Debug = 1,
			Info = 2,
			Warn = 3,
			Error = 4,
			Critical = 5,
			Off = 6,
		};
	public:
		struct FileContext
		{
			std::string _Name = "app.log";
			Level _Level = Trace;

			bool _RotateOnOpen = true;
			size_t _MaxFiles = 0;
			size_t _MaxSize = BM_MB(5);

			inline FileContext() noexcept = default;
			inline FileContext(std::string_view name, Level level = Trace,
				size_t maxFiles = 0, size_t maxSize = BM_MB(5), bool rotateOnOpen = true) noexcept
				: _Name(name), _Level(level), _MaxFiles(maxFiles), _MaxSize(maxSize), _RotateOnOpen(rotateOnOpen) {
			}
		};
	public:
		inline Log() noexcept = delete;
		inline Log(const Log&) noexcept = delete;
		inline Log(Log&&) noexcept = delete;

		static void AddConsoleSink(Level level = Trace, bool core = true, bool app = true, const std::string& pattern = "") noexcept;
		static void AddFileSink(const FileContext& context, bool core = true, bool app = true, const std::string& pattern = "") noexcept;

		static void Init(const std::string& loggerName = "App", Level flushOn = Warn, Level consoleLevel = Off) noexcept;

		static void CoreLog(Level level, const std::string& message) noexcept;
		static void AppLog(Level level, const std::string& message) noexcept;
	private:
		static inline const char* s_Pattern = "[%Y-%m-%d][%T] %^[%L] <%n> %v.%$";
	};
}

#ifdef BM_ENABLE_LOG

#define BM_LOG_CONSOLE(...)				BM::Log::AddConsoleSink(__VA_ARGS__)
#define BM_LOG_FILE(name, level, ...)	BM::Log::AddFileSink({ name, level }, __VA_ARGS__)
#define BM_LOG_INIT(...)				BM::Log::Init(__VA_ARGS__)

#define BM_CORE_TRACE(...)				BM::Log::CoreLog(BM::Log::Trace, std::format(__VA_ARGS__))
#define BM_CORE_DEBUG(...)				BM::Log::CoreLog(BM::Log::Debug, std::format(__VA_ARGS__))
#define BM_CORE_INFO(...)				BM::Log::CoreLog(BM::Log::Info, std::format(__VA_ARGS__))
#define BM_CORE_WARN(...)				BM::Log::CoreLog(BM::Log::Warn, std::format(__VA_ARGS__))
#define BM_CORE_ERROR(...)				BM::Log::CoreLog(BM::Log::Error, std::format(__VA_ARGS__))
#define BM_CORE_CRITICAL(...)			BM::Log::CoreLog(BM::Log::Critical, std::format(__VA_ARGS__))

#define BM_TRACE(...)					BM::Log::AppLog(BM::Log::Trace, std::format(__VA_ARGS__))
#define BM_DEBUG(...)					BM::Log::AppLog(BM::Log::Debug, std::format(__VA_ARGS__))
#define BM_INFO(...)					BM::Log::AppLog(BM::Log::Info, std::format(__VA_ARGS__))
#define BM_WARN(...)					BM::Log::AppLog(BM::Log::Warn, std::format(__VA_ARGS__))
#define BM_ERROR(...)					BM::Log::AppLog(BM::Log::Error, std::format(__VA_ARGS__))
#define BM_CRITICAL(...)				BM::Log::AppLog(BM::Log::Critical, std::format(__VA_ARGS__))

#define BM_CORE_FN(...)				BM_CORE_DEBUG("{}({})", __FUNCTION__, std::format(""##__VA_ARGS__))
#define BM_FUNC(...)					BM_DEBUG("{}({})", __FUNCTION__, std::format(""##__VA_ARGS__))

#else

#define BM_LOG_CONSOLE(...)
#define BM_LOG_FILE(...)
#define BM_LOG_INIT(...)

#define BM_CORE_TRACE(...)
#define BM_CORE_DEBUG(...)
#define BM_CORE_INFO(...)
#define BM_CORE_WARN(...)
#define BM_CORE_ERROR(...)
#define BM_CORE_CRITICAL(...)

#define BM_TRACE(...)
#define BM_DEBUG(...)
#define BM_INFO(...)
#define BM_WARN(...)
#define BM_ERROR(...)
#define BM_CRITICAL(...)

#define BM_CORE_FN(...)
#define BM_FUNC(...)

#endif
