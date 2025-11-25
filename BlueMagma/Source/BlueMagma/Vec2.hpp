#pragma once
#include "Assert.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <format>

namespace BM
{
	template<typename TValue>
	struct Vec2
	{
		TValue _X, _Y;

		constexpr Vec2() noexcept = default;
		template<typename UValue = TValue>
		constexpr Vec2(const UValue& x, const UValue& y) noexcept
			: _X(static_cast<TValue>(x)), _Y(static_cast<TValue>(y)) {
		}
		template<typename UValue = TValue>
		constexpr Vec2(const Vec2<UValue>& vec) noexcept
			: _X(static_cast<TValue>(vec._X)), _Y(static_cast<TValue>(vec._Y)) {
		}
		template<typename UValue = TValue>
		constexpr Vec2(const UValue& value) noexcept
			: _X(static_cast<TValue>(value)), _Y(static_cast<TValue>(value)) {
		}

		template<typename UValue = TValue>
		constexpr Vec2(const sf::Vector2<UValue>& vec) noexcept
			: _X(static_cast<TValue>(vec.x)), _Y(static_cast<TValue>(vec.y)) {
		}
		template<typename UValue = TValue>
		constexpr operator sf::Vector2<UValue>() const noexcept {
			return sf::Vector2<UValue>(static_cast<UValue>(_X), static_cast<UValue>(_Y));
		}

		constexpr TValue Area() const noexcept {
			return _X * _Y;
		}
		constexpr Vec2<float> Center() const noexcept {
			return Vec2<float>(*this) / 2.f;
		}
		constexpr TValue SquaredLength() const noexcept {
			return (_X * _X) + (_Y * _Y);
		}
		constexpr TValue Length() const noexcept {
			return sqrt(SquaredLength());
		}
		constexpr Vec2 Normalized() const noexcept {
			BM_CORE_ASSERT(*this != Zero());
			return *this / Length();
		}

		constexpr bool operator==(const Vec2& vec) const noexcept {
			return _X == vec._X && _Y == vec._Y;
		}
		constexpr bool operator==(const TValue& value) const noexcept {
			return _X == value && _Y == value;
		}

		constexpr bool operator!=(const Vec2& vec) const noexcept {
			return !(*this == vec);
		}
		constexpr bool operator!=(const TValue& value) const noexcept {
			return !(*this == value);
		}

		constexpr Vec2 operator+(const Vec2& vec) const noexcept {
			return Vec2(_X + vec._X, _Y + vec._Y);
		}
		constexpr Vec2 operator-(const Vec2& vec) const noexcept {
			return Vec2(_X - vec._X, _Y - vec._Y);
		}
		constexpr Vec2 operator*(const Vec2& vec) const noexcept {
			return Vec2(_X * vec._X, _Y * vec._Y);
		}
		constexpr Vec2 operator/(const Vec2& vec) const noexcept {
			return Vec2(_X / vec._X, _Y / vec._Y);
		}

		constexpr Vec2 operator+(const TValue& value) const noexcept {
			return Vec2(_X + value, _Y + value);
		}
		constexpr Vec2 operator-(const TValue& value) const noexcept {
			return Vec2(_X - value, _Y - value);
		}
		constexpr Vec2 operator*(const TValue& value) const noexcept {
			return Vec2(_X * value, _Y * value);
		}
		constexpr Vec2 operator/(const TValue& value) const noexcept {
			return Vec2(_X / value, _Y / value);
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

		static constexpr Vec2<TValue> Zero() noexcept {
			return Vec2<TValue>(0, 0);
		};

		static constexpr Vec2<TValue> Up() noexcept {
			return Vec2<TValue>(0, -1);
		};
		static constexpr Vec2<TValue> Down() noexcept {
			return Vec2<TValue>(0, 1);
		};
		static constexpr Vec2<TValue> Left() noexcept {
			return Vec2<TValue>(-1, 0);
		};
		static constexpr Vec2<TValue> Right() noexcept {
			return Vec2<TValue>(1, 0);
		};
	};

	using Vec2i = Vec2<int>;
	using Vec2u = Vec2<uint32_t>;
	using Vec2f = Vec2<float>;
};

namespace std
{
	template<typename TValue>
	class formatter<BM::Vec2<TValue>>
	{
	public:
		constexpr auto parse(auto& context) {
			auto it = context.begin();
			if (it == context.end())
				return it;

			if (*it == '#')
			{
				++it;
				m_Curly = true;
			}

			if (it != context.end() && *it != '}')
				throw std::format_error("Invalid format args for BM::Vec2<TValue>");

			return it;
		}

		inline auto format(const BM::Vec2<TValue>& vec, auto& context) const noexcept {
			return std::format_to(context.out(), "{}{}, {}{}", m_Curly ? "{" : "", vec._X, vec._Y, m_Curly ? "}" : "");
		}
	private:
		bool m_Curly = false;
	};
}
