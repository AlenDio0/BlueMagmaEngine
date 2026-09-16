#pragma once
#include <algorithm>
#include <string>
#include <string_view>
#include <format>
#include <ranges>

namespace BM::Utils
{
	[[nodiscard]] float InverseLerp(float current, float from, float to) noexcept;

	template<typename TValue>
	[[nodiscard]] static constexpr TValue LerpUnclamped(TValue from, TValue to, float progress) noexcept {
		return from * (1.0f - progress) + to * progress;
	}

	template<typename TValue>
	[[nodiscard]] static constexpr TValue Lerp(TValue from, TValue to, float progress) noexcept {
		return LerpUnclamped(from, to, std::clamp(progress, 0.f, 1.f));
	}

	[[nodiscard]] constexpr bool IsSpace(char c) noexcept {
		return std::isspace(c);
	}

	[[nodiscard]] constexpr bool IsNotSpace(char c) noexcept {
		return !IsSpace(c);
	}

	[[nodiscard]] static constexpr std::string JoinWith(auto&& range, std::string_view divider) noexcept {
		auto view = range | std::views::join_with(divider) | std::views::common;
		return std::string(view.begin(), view.end());
	}

	[[nodiscard]] static constexpr auto SplitWith(std::string_view str, std::string_view divider) noexcept {
		return str | std::views::split(divider);
	}

	[[nodiscard]] constexpr std::string_view Trim(std::string_view str) noexcept {
		auto begin = std::ranges::find_if(str, IsNotSpace);
		auto end = std::ranges::find_if(str | std::views::reverse, IsNotSpace).base();
		return std::string_view(begin, end);
	}
}
