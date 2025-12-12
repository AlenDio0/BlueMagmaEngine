#pragma once
#include <filesystem>
#include <string>
#include <format>

// Converts Megabyte in Bytes
#define BM_MB(x) x * 1024 * 1024

// Gets the current file name
#define BM_FILENAME std::filesystem::path(__FILE__).filename().string()

#if _DEBUG || true
#define BM_ENABLE_LOG
#endif

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
			size_t _MaxFiles = 0;
			size_t _MaxSize = BM_MB(5);
			bool _RotateOnOpen = true;

			inline FileContext() noexcept = default;
			inline FileContext(std::string_view name) noexcept
				: _Name(name) {
			}
			inline FileContext(std::string_view name, size_t maxFiles) noexcept
				: _Name(name), _MaxFiles(maxFiles) {
			}
			inline FileContext(std::string_view name, size_t maxFiles, size_t maxSize, bool rotateOnOpen = true) noexcept
				: _Name(name), _MaxFiles(maxFiles), _MaxSize(maxSize), _RotateOnOpen(rotateOnOpen) {
			}
		};
	public:
		inline Log() noexcept = delete;
		inline Log(const Log&) noexcept = delete;
		inline Log(Log&&) noexcept = delete;

		static void AddConsoleSink(Level core = Trace, Level app = Trace, const std::string& pattern = "") noexcept;
		static void AddFileSink(const FileContext& context, Level core = Trace, Level app = Trace, const std::string& pattern = "") noexcept;

		static void Init(const std::string& loggerName = "App", Level flushOn = Warn, Level consoleLevel = Off) noexcept;

		static void CoreLog(Level level, const std::string& message) noexcept;
		static void AppLog(Level level, const std::string& message) noexcept;
	private:
		static inline const char* s_Pattern = "[%Y-%m-%d][%T] %^[%-5l] <%n> %v.%$";
	};
}

#ifdef BM_ENABLE_LOG

#define BM_LOG_INIT(...)				BM::Log::Init(__VA_ARGS__)

#define BM_LOG(type, level, ...)		BM::Log::type##Log(BM::Log::level, std::format(__VA_ARGS__))
#define BM_LOG_FN(type, ...)			BM##type##DEBUG("{}({}) [{}:{}]", __FUNCTION__, std::format("" __VA_ARGS__), BM_FILENAME, __LINE__)

#else

#define BM_LOG_CONSOLE(...)
#define BM_LOG_FILE(...)
#define BM_LOG_INIT(...)

#define BM_LOG(...)
#define BM_LOG_FN(...)

#endif

#define BM_CORE_TRACE(...)		BM_LOG(Core, Trace, __VA_ARGS__)
#define BM_CORE_DEBUG(...)		BM_LOG(Core, Debug, __VA_ARGS__)
#define BM_CORE_INFO(...)		BM_LOG(Core, Info, __VA_ARGS__)
#define BM_CORE_WARN(...)		BM_LOG(Core, Warn, __VA_ARGS__)
#define BM_CORE_ERROR(...)		BM_LOG(Core, Error, __VA_ARGS__)
#define BM_CORE_CRITICAL(...)	BM_LOG(Core, Critical, __VA_ARGS__)

#define BM_TRACE(...)			BM_LOG(App, Trace, __VA_ARGS__)
#define BM_DEBUG(...)			BM_LOG(App, Debug, __VA_ARGS__)
#define BM_INFO(...)			BM_LOG(App, Info, __VA_ARGS__)
#define BM_WARN(...)			BM_LOG(App, Warn, __VA_ARGS__)
#define BM_ERROR(...)			BM_LOG(App, Error, __VA_ARGS__)
#define BM_CRITICAL(...)		BM_LOG(App, Critical, __VA_ARGS__)

#define BM_CORE_FN(...)			BM_LOG_FN(_CORE_, __VA_ARGS__)
#define BM_FN(...)				BM_LOG_FN(_, __VA_ARGS__)
