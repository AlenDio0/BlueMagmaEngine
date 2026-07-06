#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdint>
#include <format>

namespace BM
{
	template<typename TValue>
	struct Vec2
	{
		TValue X{}, Y{};

		constexpr Vec2() noexcept = default;
		template<typename UValue = TValue>
		constexpr Vec2(const UValue& x, const UValue& y) noexcept
			: X(static_cast<TValue>(x)), Y(static_cast<TValue>(y)) {}
		template<typename UValue = TValue>
		constexpr Vec2(const Vec2<UValue>& vec) noexcept
			: X(static_cast<TValue>(vec.X)), Y(static_cast<TValue>(vec.Y)) {}
		template<typename UValue = TValue>
		constexpr explicit Vec2(const UValue& value) noexcept
			: X(static_cast<TValue>(value)), Y(static_cast<TValue>(value)) {}

		template<typename UValue = TValue>
		constexpr Vec2(const sf::Vector2<UValue>& vec) noexcept
			: X(static_cast<TValue>(vec.x)), Y(static_cast<TValue>(vec.y)) {}
		template<typename UValue = TValue>
		constexpr operator sf::Vector2<UValue>() const noexcept {
			return sf::Vector2<UValue>(static_cast<UValue>(X), static_cast<UValue>(Y));
		}

		constexpr Vec2<float> Center() const noexcept {
			return Vec2<float>(*this) / 2.f;
		}
		constexpr Vec2 Normalized() const noexcept {
			if (*this == Zero())
				return Zero();

			return *this / Length();
		}
		constexpr Vec2 Squared() const noexcept {
			return (*this) * (*this);
		}
		constexpr Vec2 Round() const noexcept {
			return Vec2(std::round(X), std::round(Y));
		}
		constexpr Vec2 Absolute() const noexcept {
			return Vec2(std::abs(X), std::abs(Y));
		}
		constexpr Vec2 Swap() const noexcept {
			return Vec2(Y, X);
		}
		constexpr Vec2 Rotate() const noexcept {
			return Vec2(Y, -X);
		}

		constexpr TValue Sum() const noexcept {
			return X + Y;
		}
		constexpr TValue Area() const noexcept {
			return X * Y;
		}

		constexpr TValue Dot(const Vec2& vec) const noexcept {
			return Vec2(*this * vec).Sum();
		}

		constexpr TValue SquaredLength() const noexcept {
			return Squared().Sum();
		}
		constexpr TValue Length() const noexcept {
			return sqrt(SquaredLength());
		}

		constexpr TValue SquaredDistance(const Vec2& vec) const noexcept {
			return Vec2(*this - vec).Squared().Sum();
		}
		constexpr TValue Distance(const Vec2& vec) const noexcept {
			return sqrt(SquaredDistance(vec));
		}

		constexpr bool operator==(const Vec2& vec) const noexcept {
			return X == vec.X && Y == vec.Y;
		}
		constexpr bool operator==(const TValue& value) const noexcept {
			return X == value && Y == value;
		}

		constexpr bool operator!=(const Vec2& vec) const noexcept {
			return !(*this == vec);
		}
		constexpr bool operator!=(const TValue& value) const noexcept {
			return !(*this == value);
		}

		constexpr Vec2 operator+(const Vec2& vec) const noexcept {
			return Vec2(X + vec.X, Y + vec.Y);
		}
		constexpr Vec2 operator-(const Vec2& vec) const noexcept {
			return Vec2(X - vec.X, Y - vec.Y);
		}
		constexpr Vec2 operator*(const Vec2& vec) const noexcept {
			return Vec2(X * vec.X, Y * vec.Y);
		}
		constexpr Vec2 operator/(const Vec2& vec) const noexcept {
			return Vec2(X / vec.X, Y / vec.Y);
		}

		constexpr Vec2 operator+(const TValue& value) const noexcept {
			return Vec2(X + value, Y + value);
		}
		constexpr Vec2 operator-(const TValue& value) const noexcept {
			return Vec2(X - value, Y - value);
		}
		constexpr Vec2 operator*(const TValue& value) const noexcept {
			return Vec2(X * value, Y * value);
		}
		constexpr Vec2 operator/(const TValue& value) const noexcept {
			return Vec2(X / value, Y / value);
		}

		constexpr Vec2& operator+=(const Vec2& vec) noexcept {
			*this = *this + vec;
			return *this;
		}
		constexpr Vec2& operator-=(const Vec2& vec) noexcept {
			*this = *this - vec;
			return *this;
		}
		constexpr Vec2& operator*=(const Vec2& vec) noexcept {
			*this = *this * vec;
			return *this;
		}
		constexpr Vec2& operator/=(const Vec2& vec) noexcept {
			*this = *this / vec;
			return *this;
		}

		constexpr Vec2& operator+=(const TValue& value) noexcept {
			*this = *this + value;
			return *this;
		}
		constexpr Vec2& operator-=(const TValue& value) noexcept {
			*this = *this - value;
			return *this;
		}
		constexpr Vec2& operator*=(const TValue& value) noexcept {
			*this = *this * value;
			return *this;
		}
		constexpr Vec2& operator/=(const TValue& value) noexcept {
			*this = *this / value;
			return *this;
		}

		static constexpr Vec2 Zero() noexcept {
			return Vec2(0);
		};

		static constexpr Vec2 Up() noexcept {
			return Vec2(0, -1);
		};
		static constexpr Vec2 Down() noexcept {
			return Vec2(0, 1);
		};
		static constexpr Vec2 Left() noexcept {
			return Vec2(-1, 0);
		};
		static constexpr Vec2 Right() noexcept {
			return Vec2(1, 0);
		};
	};

	using Vec2i = Vec2<int>;
	using Vec2u = Vec2<uint32_t>;
	using Vec2f = Vec2<float>;
};

namespace std
{
	template<typename TValue>
	struct formatter<BM::Vec2<TValue>>
	{
		constexpr auto parse(auto& context) {
			return context.begin();
		}

		inline auto format(const BM::Vec2<TValue>& vec, auto& context) const noexcept {
			return std::format_to(context.out(), "[{}, {}]", vec.X, vec.Y);
		}
	};
}
