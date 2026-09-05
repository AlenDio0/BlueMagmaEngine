#include "bmpch.hpp"
#include "Timer.hpp"

namespace BM
{
	Timer::Timer() noexcept
		: m_StartPoint(Now())
	{
	}

	Timer Timer::Restart() noexcept
	{
		Timer copy = *this;

		m_StartPoint = Now();

		return copy;
	}

	float Timer::AsSeconds() const noexcept
	{
		return AsMilli() / 1000.f;
	}

	float Timer::AsMilli() const noexcept
	{
		return static_cast<float>(AsMicro()) / 1000.f;
	}

	int64_t Timer::AsMicro() const noexcept
	{
		return AsNano() / 1000ll;
	}

	int64_t Timer::AsNano() const noexcept
	{
		return Elapsed().count();
	}

	std::chrono::nanoseconds Timer::Elapsed() const noexcept
	{
		return Now() - m_StartPoint;
	}

	std::chrono::steady_clock::time_point Timer::Now() noexcept
	{
		return std::chrono::steady_clock::now();
	}

	//======================================================================================

	ScopeTimer::ScopeTimer(std::string_view label) noexcept
		: m_Label(label)
	{
	}

	ScopeTimer::~ScopeTimer() noexcept
	{
		const float cElapsedSeconds = m_Timer.AsSeconds();
		const float cElapsedMs = m_Timer.AsMilli();

		BM_CORE_INFO("Timer Stopped - {} took {:.3f}s ({}ms) to finish", m_Label, cElapsedSeconds, cElapsedMs);
	}
}
