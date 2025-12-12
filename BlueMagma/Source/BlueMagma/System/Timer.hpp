#pragma once
#include <chrono>
#include <cstdint>

namespace BM
{
	class Timer
	{
	public:
		Timer() noexcept;

		Timer Restart() noexcept;

		float AsSeconds() const noexcept;
		float AsMilli() const noexcept;
		int64_t AsMicro() const noexcept;
		int64_t AsNano() const noexcept;

		std::chrono::nanoseconds Elapsed() const noexcept;

		static std::chrono::steady_clock::time_point Now() noexcept;
	private:
		std::chrono::steady_clock::time_point m_StartPoint;
	};

	class ScopeTimer
	{
	public:
		ScopeTimer(std::string_view message) noexcept;
		~ScopeTimer() noexcept;
	private:
		Timer m_Timer;
		std::string m_Message;
	};
}
