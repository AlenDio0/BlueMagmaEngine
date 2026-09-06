#pragma once
#include "Utils.hpp"

#include <string>
#include <filesystem>
#include <format>

// Converts Megabyte in Bytes
#define BM_MB(x) ((x) * 1024 * 1024)

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
			std::string Name = "app.log";
			size_t MaxFiles = 0;
			size_t MaxSize = BM_MB(5);
			bool RotateOnOpen = true;
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

	//======================================================================================

	template<typename... TArgs>
	constexpr std::string UnpackFnArgs(std::string_view argNames, TArgs&&... args) noexcept {
		if constexpr (sizeof...(TArgs) == 0)
			return "";

		auto names = Utils::SplitWith(argNames, ",");
		auto iterator = names.begin();

		std::string unpacked;
		auto unpackArg = [&](const auto& argValue) {
			if (!unpacked.empty())
				unpacked += ", ";

			std::string_view currentArg{ (*iterator).begin(), (*iterator).end() };

			unpacked += std::format("{}: '{}'", Utils::Trim(currentArg), argValue);
			iterator++;
			};

		(unpackArg(std::forward<TArgs>(args)), ...);
		return unpacked;
	}
}

//======================================================================================

#ifdef BM_ENABLE_LOG

#define BM_LOG(type, level, ...)			BM::Log::type##Log(BM::Log::level, std::format(__VA_ARGS__))
#define BM_LOG_FN(type, level, ...)			BM##type##level("{}(...) {} [{}:{}]", __FUNCTION__, std::format("" __VA_ARGS__), BM_FILENAME, __LINE__)
#define BM_LOG_FN_ARGS(type, level, ...)	BM##type##level("{}({}) [{}:{}]", __FUNCTION__, BM::UnpackFnArgs(#__VA_ARGS__, __VA_ARGS__), BM_FILENAME, __LINE__)

//======================================================================================

#else

#define BM_LOG(...)
#define BM_LOG_FN(...)
#define BM_LOG_FN_ARGS(...)

#endif

//======================================================================================

#define BM_LOG_CORE(level, ...)	BM_LOG(Core, level, __VA_ARGS__)

#define BM_CORE_TRACE(...)		BM_LOG_CORE(Trace, __VA_ARGS__)
#define BM_CORE_DEBUG(...)		BM_LOG_CORE(Debug, __VA_ARGS__)
#define BM_CORE_INFO(...)		BM_LOG_CORE(Info, __VA_ARGS__)
#define BM_CORE_WARN(...)		BM_LOG_CORE(Warn, __VA_ARGS__)
#define BM_CORE_ERROR(...)		BM_LOG_CORE(Error, __VA_ARGS__)
#define BM_CORE_CRITICAL(...)	BM_LOG_CORE(Critical, __VA_ARGS__)

#define BM_TRACE(...)			BM_LOG(App, Trace, __VA_ARGS__)
#define BM_DEBUG(...)			BM_LOG(App, Debug, __VA_ARGS__)
#define BM_INFO(...)			BM_LOG(App, Info, __VA_ARGS__)
#define BM_WARN(...)			BM_LOG(App, Warn, __VA_ARGS__)
#define BM_ERROR(...)			BM_LOG(App, Error, __VA_ARGS__)
#define BM_CRITICAL(...)		BM_LOG(App, Critical, __VA_ARGS__)

//======================================================================================

#define BM_LOG_CORE_FN(level, ...)	BM_LOG_FN(_CORE_, level, __VA_ARGS__)

#define BM_CORE_TRACE_FN(...)		BM_LOG_CORE_FN(TRACE, __VA_ARGS__)
#define BM_CORE_DEBUG_FN(...)		BM_LOG_CORE_FN(DEBUG, __VA_ARGS__)
#define BM_CORE_INFO_FN(...)		BM_LOG_CORE_FN(INFO, __VA_ARGS__)
#define BM_CORE_WARN_FN(...)		BM_LOG_CORE_FN(WARN, __VA_ARGS__)
#define BM_CORE_ERROR_FN(...)		BM_LOG_CORE_FN(ERROR, __VA_ARGS__)
#define BM_CORE_CRITICAL_FN(...)	BM_LOG_CORE_FN(CRITICAL, __VA_ARGS__)

#define BM_TRACE_FN(...)			BM_LOG_FN(_, TRACE, __VA_ARGS__)
#define BM_DEBUG_FN(...)			BM_LOG_FN(_, DEBUG, __VA_ARGS__)
#define BM_INFO_FN(...)				BM_LOG_FN(_, INFO, __VA_ARGS__)
#define BM_WARN_FN(...)				BM_LOG_FN(_, WARN, __VA_ARGS__)
#define BM_ERROR_FN(...)			BM_LOG_FN(_, ERROR, __VA_ARGS__)
#define BM_CRITICAL_FN(...)			BM_LOG_FN(_, CRITICAL, __VA_ARGS__)

#define BM_CORE_FN(...)				BM_CORE_TRACE_FN(__VA_ARGS__)
#define BM_FN(...)					BM_TRACE_FN(__VA_ARGS__)

//======================================================================================

#define BM_LOG_CORE_FN_ARGS(level, ...)	BM_LOG_FN_ARGS(_CORE_, level, __VA_ARGS__)

#define BM_CORE_TRACE_FN_ARGS(...)		BM_LOG_CORE_FN_ARGS(TRACE, __VA_ARGS__)
#define BM_CORE_DEBUG_FN_ARGS(...)		BM_LOG_CORE_FN_ARGS(DEBUG, __VA_ARGS__)
#define BM_CORE_INFO_FN_ARGS(...)		BM_LOG_CORE_FN_ARGS(INFO, __VA_ARGS__)
#define BM_CORE_WARN_FN_ARGS(...)		BM_LOG_CORE_FN_ARGS(WARN, __VA_ARGS__)
#define BM_CORE_ERROR_FN_ARGS(...)		BM_LOG_CORE_FN_ARGS(ERROR, __VA_ARGS__)
#define BM_CORE_CRITICAL_FN_ARGS(...)	BM_LOG_CORE_FN_ARGS(CRITICAL, __VA_ARGS__)

#define BM_TRACE_FN_ARGS(...)			BM_LOG_FN_ARGS(_, TRACE, __VA_ARGS__)
#define BM_DEBUG_FN_ARGS(...)			BM_LOG_FN_ARGS(_, DEBUG, __VA_ARGS__)
#define BM_INFO_FN_ARGS(...)			BM_LOG_FN_ARGS(_, INFO, __VA_ARGS__)
#define BM_WARN_FN_ARGS(...)			BM_LOG_FN_ARGS(_, WARN, __VA_ARGS__)
#define BM_ERROR_FN_ARGS(...)			BM_LOG_FN_ARGS(_, ERROR, __VA_ARGS__)
#define BM_CRITICAL_FN_ARGS(...)		BM_LOG_FN_ARGS(_, CRITICAL, __VA_ARGS__)

#define BM_CORE_FN_ARGS(...)			BM_CORE_TRACE_FN_ARGS(__VA_ARGS__)
#define BM_FN_ARGS(...)					BM_TRACE_FN_ARGS(__VA_ARGS__)
