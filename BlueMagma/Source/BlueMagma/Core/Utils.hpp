#pragma once
#include <algorithm>
#include <string>
#include <string_view>
#include <format>
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

	constexpr bool IsSpace(char c) noexcept {
		return std::isspace(c);
	}

	constexpr bool IsNotSpace(char c) noexcept {
		return !IsSpace(c);
	}

	static constexpr std::string JoinWith(auto&& range, std::string_view divider) noexcept {
		auto view = range | std::views::join_with(divider) | std::views::common;
		return std::string(view.begin(), view.end());
	}

	static constexpr auto SplitWith(std::string_view str, std::string_view divider) noexcept {
		return str | std::views::split(divider);
	}

	constexpr std::string_view Trim(std::string_view str) noexcept {
		auto begin = std::ranges::find_if(str, IsNotSpace);
		auto end = std::ranges::find_if(str | std::views::reverse, IsNotSpace).base();
		return std::string_view(begin, end);
	}
}
