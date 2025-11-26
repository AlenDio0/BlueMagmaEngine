#pragma once
#include <string>
#include <format>

#if _DEBUG
#define BM_ENABLE_LOG
#endif

namespace BM
{
	namespace Log
	{
		enum class Level
		{
			Trace = 0,
			Debug = 1,
			Info = 2,
			Warn = 3,
			Error = 4,
			Critica = 5,
			Off = 6,
		};

		void SetLevel(Level level) noexcept;
		void SetPattern(const std::string& pattern) noexcept;

		void CoreLog(Level level, const std::string& message) noexcept;
		void AppLog(Level level, const std::string& message) noexcept;
	}
}

#ifdef BM_ENABLE_LOG

#define BM_CORE_TRACE(...)		BM::Log::CoreLog(BM::Log::Level::Trace, std::format(__VA_ARGS__))
#define BM_CORE_DEBUG(...)		BM::Log::CoreLog(BM::Log::Level::Debug, std::format(__VA_ARGS__))
#define BM_CORE_INFO(...)		BM::Log::CoreLog(BM::Log::Level::Info, std::format(__VA_ARGS__))
#define BM_CORE_WARN(...)		BM::Log::CoreLog(BM::Log::Level::Warn, std::format(__VA_ARGS__))
#define BM_CORE_ERROR(...)		BM::Log::CoreLog(BM::Log::Level::Error, std::format(__VA_ARGS__))
#define BM_CORE_CRITICAL(...)	BM::Log::CoreLog(BM::Log::Level::Critical, std::format(__VA_ARGS__))

#define BM_TRACE(...)			BM::Log::AppLog(BM::Log::Level::Trace, std::format(__VA_ARGS__))
#define BM_DEBUG(...)			BM::Log::AppLog(BM::Log::Level::Debug, std::format(__VA_ARGS__))
#define BM_INFO(...)			BM::Log::AppLog(BM::Log::Level::Info, std::format(__VA_ARGS__))
#define BM_WARN(...)			BM::Log::AppLog(BM::Log::Level::Warn, std::format(__VA_ARGS__))
#define BM_ERROR(...)			BM::Log::AppLog(BM::Log::Level::Error, std::format(__VA_ARGS__))
#define BM_CRITICAL(...)		BM::Log::AppLog(BM::Log::Level::Critical, std::format(__VA_ARGS__))

#define BM_CORE_FUNC(...)		BM_CORE_DEBUG("{}({})", __FUNCTION__, std::format(""##__VA_ARGS__))
#define BM_FUNC(...)			BM_DEBUG("{}({})", __FUNCTION__, std::format(""##__VA_ARGS__))

#else

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

#define BM_CORE_FUNC(...)
#define BM_FUNC(...)

#endif
