#pragma once
#include "Utils.hpp"
#include <SFML/Graphics/Color.hpp>
#include <cstdint>

namespace BM
{
	namespace ColorImpl
	{
		static constexpr uint8_t IntToChannel(int value) noexcept {
			return static_cast<uint8_t>(std::clamp(value, 0, 255));
		}
		static constexpr uint8_t FloatToChannel(float value) noexcept {
			return static_cast<uint8_t>(value + 0.5f);
		}
	}

	struct Color
	{
		union
		{
			uint32_t RGBA = 0xFFFFFFFF;
			struct
			{
				uint8_t Alpha;
				uint8_t Blue;
				uint8_t Green;
				uint8_t Red;
			};
		};

		constexpr Color() noexcept = default;
		constexpr Color(uint32_t rgba) noexcept
			: RGBA(rgba) {
		}
		constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255u) noexcept
			: Red(red), Green(green), Blue(blue), Alpha(alpha) {
		}
		constexpr Color(float red, float green, float blue, float alpha = 1.f) noexcept
			: Color(ColorImpl::FloatToChannel(red * 255.f), ColorImpl::FloatToChannel(green * 255.f), ColorImpl::FloatToChannel(blue * 255.f), ColorImpl::FloatToChannel(alpha * 255.f)) {
		}

		constexpr Color(const sf::Color& color) noexcept
			: RGBA(color.toInteger()) {
		}
		constexpr operator sf::Color() const noexcept {
			return sf::Color(RGBA);
		}

		constexpr bool operator==(const Color& color) const noexcept {
			return RGBA == color.RGBA;
		}

		friend constexpr Color operator+(const Color& left, const Color& right) noexcept {
			return Color(ColorImpl::IntToChannel(left.Red + right.Red), ColorImpl::IntToChannel(left.Green + right.Green),
				ColorImpl::IntToChannel(left.Blue + right.Blue), ColorImpl::IntToChannel(left.Alpha + right.Alpha));
		}

		constexpr Color operator*(float value) const noexcept {
			return Color(ColorImpl::FloatToChannel(value * Red), ColorImpl::FloatToChannel(value * Green), ColorImpl::FloatToChannel(value * Blue), ColorImpl::FloatToChannel(value * Alpha));
		}

		constexpr Color WithAlpha(uint8_t alpha) const noexcept {
			return Color(Red, Green, Blue, alpha);
		}
		constexpr Color WithAlpha(float alpha) const noexcept {
			return WithAlpha(ColorImpl::FloatToChannel(alpha * 255u));
		}

		static constexpr Color Lerp(Color fromColor, Color toColor, float progress) noexcept {
			return Utils::Lerp(fromColor, toColor, progress);
		}
		static constexpr Color LerpUnclamped(Color fromColor, Color toColor, float progress) noexcept {
			return Utils::LerpUnclamped(fromColor, toColor, progress);
		}
	};
	namespace ColorDef
	{
		inline constexpr Color Black(0.f, 0.f, 0.f);
		inline constexpr Color White(1.f, 1.f, 1.f);
		inline constexpr Color Red(1.f, 0.f, 0.f);
		inline constexpr Color Green(0.f, 1.f, 0.f);
		inline constexpr Color Blue(0.f, 0.f, 1.f);
		inline constexpr Color Yellow(1.f, 1.f, 0.f);
		inline constexpr Color Magenta(1.f, 0.f, 1.f);
		inline constexpr Color Cyan(0.f, 1.f, 1.f);
		inline constexpr Color Clear(0.f, 0.f, 0.f, 0.f);
	}
}
