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

		[[nodiscard]] float AsSeconds() const noexcept;
		[[nodiscard]] float AsMilli() const noexcept;
		[[nodiscard]] int64_t AsMicro() const noexcept;
		[[nodiscard]] int64_t AsNano() const noexcept;

		[[nodiscard]] std::chrono::nanoseconds Elapsed() const noexcept;

		[[nodiscard]] static std::chrono::steady_clock::time_point Now() noexcept;
	private:
		std::chrono::steady_clock::time_point m_StartPoint;
	};

	//======================================================================================

	class ScopeTimer
	{
	public:
		explicit ScopeTimer(std::string_view label) noexcept;
		~ScopeTimer() noexcept;
	private:
		Timer m_Timer;
		std::string m_Label;
	};
}
