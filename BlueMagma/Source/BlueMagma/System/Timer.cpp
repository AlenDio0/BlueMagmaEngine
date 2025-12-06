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

	ScopeTimer::ScopeTimer(std::string_view message) noexcept
		: m_Message(message)
	{
	}

	ScopeTimer::~ScopeTimer() noexcept
	{
		const float cElapsed = m_Timer.AsMilli();
		BM_CORE_DEBUG("{} - {}ms", m_Message, cElapsed);
	}
}
