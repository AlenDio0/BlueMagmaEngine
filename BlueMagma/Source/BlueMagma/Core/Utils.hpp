#pragma once
#include <algorithm>
#include <string>
#include <string_view>
#include <ranges>

namespace BM::Utils
{
	float InverseLerp(float current, float from, float to) noexcept;

	template<typename TValue>
	static constexpr TValue LerpUnclamped(TValue from, TValue to, float progress) noexcept {
		return from * (1.0f - progress) + to * progress;
	}

	template<typename TValue>
	static constexpr TValue Lerp(TValue from, TValue to, float progress) noexcept {
		return LerpUnclamped(from, to, std::clamp(progress, 0.f, 1.f));
	}

	template<class Range>
	static constexpr std::string JoinWith(Range&& range, std::string_view divider) noexcept {
		auto view = range | std::views::join_with(divider) | std::views::common;
		return std::string(view.begin(), view.end());
	}
}
