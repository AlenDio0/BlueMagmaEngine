#pragma once
#include "Vec2.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <format>

namespace BM
{
	template<typename TValue>
	struct Rect
	{
		union
		{
			struct
			{
				TValue X, Y;
				TValue Width, Height;
			};
			struct
			{
				Vec2<TValue> Position, Size;
			};
		};

		constexpr Rect() noexcept = default;
		template<typename UValue = TValue>
		constexpr Rect(const UValue& x, const UValue& y, const UValue& width, const UValue& height) noexcept
			: X(static_cast<TValue>(x)), Y(static_cast<TValue>(y)),
			Width(static_cast<TValue>(width)), Height(static_cast<TValue>(height)) {}
		constexpr Rect(const Vec2<TValue>& position, const Vec2<TValue>& size) noexcept
			: Position(position), Size(size) {}
		template<typename UValue = TValue>
		constexpr Rect(const Rect<UValue>& rect) noexcept
			: X(static_cast<TValue>(rect.X)), Y(static_cast<TValue>(rect.Y)),
			Width(static_cast<TValue>(rect.Width)), Height(static_cast<TValue>(rect.Height)) {}
		template<typename UValue = TValue>
		explicit constexpr Rect(const UValue& value) noexcept
			: X(static_cast<TValue>(value)), Y(static_cast<TValue>(value)),
			Width(static_cast<TValue>(value)), Height(static_cast<TValue>(value)) {}

		template<typename UValue = TValue>
		constexpr Rect(const sf::Rect<UValue>& rect) noexcept
			: Position(rect.position), Size(rect.size) {}
		template<typename UValue = TValue>
		constexpr operator sf::Rect<UValue>() const noexcept {
			return sf::Rect<UValue>(Position, Size);
		}

		constexpr TValue Area() const noexcept {
			return Size.Area();
		}
		constexpr Vec2<float> Center() const noexcept {
			return static_cast<Vec2<float>>(Position) + Size.Center();
		}
		constexpr Vec2<TValue> Min() const noexcept {
			return Position + Vec2<TValue>(Min(Size.X, 0), Min(Size.Y, 0));
		}
		constexpr Vec2<TValue> Max() const noexcept {
			return Position + Vec2<TValue>(Max(Size.X, 0), Max(Size.Y, 0));
		}

		constexpr bool Contains(const Vec2<TValue>& vec) const noexcept {
			const Vec2<TValue> min = Min(), max = Max();
			return (vec.X >= min.X && vec.Y >= min.Y) && (vec.X <= max.X && vec.Y <= max.Y);
		}
		constexpr bool Intersects(const Rect<TValue>& rect) const noexcept {
			const Vec2<TValue> min1 = Min(), min2 = rect.Min(), max1 = Max(), max2 = rect.Max();
			const TValue left = Max(min1.X, min2.X), right = Min(max1.X, max2.X);
			const TValue top = Max(min1.Y, min2.Y), bottom = Min(max1.Y, max2.Y);
			return left < right && top < bottom;
		}

		constexpr bool operator==(const Rect& rect) const noexcept {
			return Position == rect.Position && Size == rect.Size;
		}
		constexpr bool operator==(const Vec2<TValue>& vec) const noexcept {
			return Position == vec && Size == vec;
		}
		constexpr bool operator==(const TValue& value) const noexcept {
			return Position == value && Size == value;
		}

		constexpr bool operator!=(const Rect& rect) const noexcept {
			return !(*this == rect);
		}
		constexpr bool operator!=(const Vec2<TValue>& vec) const noexcept {
			return !(*this == vec);
		}
		constexpr bool operator!=(const TValue& value) const noexcept {
			return !(*this == value);
		}

		constexpr Rect operator+(const Rect& rect) const noexcept {
			return Rect(Position + rect.Position, Size + rect.Size);
		}
		constexpr Rect operator-(const Rect& rect) const noexcept {
			return Rect(Position - rect.Position, Size - rect.Size);
		}
		constexpr Rect operator*(const Rect& rect) const noexcept {
			return Rect(Position * rect.Position, Size * rect.Size);
		}
		constexpr Rect operator/(const Rect& rect) const noexcept {
			return Rect(Position / rect.Position, Size / rect.Size);
		}

		template<typename UValue = TValue>
		constexpr Rect operator+(const Vec2<UValue>& vec) const noexcept {
			return Rect(Position + vec, Size + vec);
		}
		template<typename UValue = TValue>
		constexpr Rect operator-(const Vec2<UValue>& vec) const noexcept {
			return Rect(Position - vec, Size - vec);
		}
		template<typename UValue = TValue>
		constexpr Rect operator*(const Vec2<UValue>& vec) const noexcept {
			return Rect(Position * vec, Size * vec);
		}
		template<typename UValue = TValue>
		constexpr Rect operator/(const Vec2<UValue>& vec) const noexcept {
			return Rect(Position / vec, Size / vec);
		}

		constexpr Rect operator+(const TValue& value) const noexcept {
			return Rect(Position + value, Size + value);
		}
		constexpr Rect operator-(const TValue& value) const noexcept {
			return Rect(Position - value, Size - value);
		}
		constexpr Rect operator*(const TValue& value) const noexcept {
			return Rect(Position * value, Size * value);
		}
		constexpr Rect operator/(const TValue& value) const noexcept {
			return Rect(Position / value, Size / value);
		}

		static constexpr Rect<TValue> Zero() noexcept {
			return Rect<TValue>(0);
		}
	private:
		constexpr static inline TValue Min(const TValue& a, const TValue& b) noexcept {
			return a < b ? a : b;
		}
		constexpr static inline TValue Max(const TValue& a, const TValue& b) noexcept {
			return a < b ? b : a;
		}
	};

	using RectInt = Rect<int>;
	using RectFloat = Rect<float>;
};

namespace std
{
	template<typename TValue>
	struct formatter<BM::Rect<TValue>>
	{
		constexpr auto parse(auto& context) {
			return context.begin();
		}

		inline auto format(const BM::Rect<TValue>& rect, auto& context) const noexcept {
			return std::format_to(context.out(), "{{{}, {}}}", rect.Position, rect.Size);
		}
	};
}
