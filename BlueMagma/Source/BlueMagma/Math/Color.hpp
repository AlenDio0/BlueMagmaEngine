#pragma once
#include <SFML/Graphics/Color.hpp>

#include <cstdint>

namespace BM
{
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
			: Color(FloatToChannel(red * 255.f), FloatToChannel(green * 255.f), FloatToChannel(blue * 255.f), FloatToChannel(alpha * 255.f)) {
		}

		constexpr Color(const sf::Color& color) noexcept
			: RGBA(color.toInteger()) {
		}
		constexpr operator sf::Color() const noexcept {
			return sf::Color(RGBA);
		}

		//======================================================================================

		[[nodiscard]] constexpr bool operator==(const Color& color) const noexcept {
			return RGBA == color.RGBA;
		}

		//======================================================================================

		[[nodiscard]] constexpr Color operator+(const Color& right) noexcept {
			return Color(IntToChannel(Red + right.Red), IntToChannel(Green + right.Green),
				IntToChannel(Blue + right.Blue), IntToChannel(Alpha + right.Alpha));
		}

		[[nodiscard]] constexpr Color operator*(float value) const noexcept {
			return Color(FloatToChannel(value * Red), FloatToChannel(value * Green), FloatToChannel(value * Blue), FloatToChannel(value * Alpha));
		}

		//======================================================================================

		[[nodiscard]] constexpr Color WithAlpha(uint8_t alpha) const noexcept {
			return Color(Red, Green, Blue, alpha);
		}
		[[nodiscard]] constexpr Color WithAlpha(float alpha) const noexcept {
			return WithAlpha(FloatToChannel(alpha * 255u));
		}

	private:
		[[nodiscard]] static constexpr uint8_t IntToChannel(int value) noexcept {
			return static_cast<uint8_t>(std::clamp(value, 0, 255));
		}
		[[nodiscard]] static constexpr uint8_t FloatToChannel(float value) noexcept {
			return static_cast<uint8_t>(value + 0.5f);
		}
	};

	namespace ColorDef
	{
		constexpr inline Color Black(0.f, 0.f, 0.f);
		constexpr inline Color White(1.f, 1.f, 1.f);
		constexpr inline Color Red(1.f, 0.f, 0.f);
		constexpr inline Color Green(0.f, 1.f, 0.f);
		constexpr inline Color Blue(0.f, 0.f, 1.f);
		constexpr inline Color Yellow(1.f, 1.f, 0.f);
		constexpr inline Color Magenta(1.f, 0.f, 1.f);
		constexpr inline Color Cyan(0.f, 1.f, 1.f);
		constexpr inline Color Clear(0.f, 0.f, 0.f, 0.f);
	}
}
