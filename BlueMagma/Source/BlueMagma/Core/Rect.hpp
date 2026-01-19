#pragma once
#include "Vec2.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
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
				TValue _X, _Y;
				TValue _Width, _Height;
			};
			struct
			{
				Vec2<TValue> _Position, _Size;
			};
		};

		constexpr Rect() noexcept = default;
		template<typename UValue = TValue>
		constexpr Rect(const UValue& x, const UValue& y, const UValue& width, const UValue& height) noexcept
			: _X(static_cast<TValue>(x)), _Y(static_cast<TValue>(y)),
			_Width(static_cast<TValue>(width)), _Height(static_cast<TValue>(height)) {
		}
		constexpr Rect(const Vec2<TValue>& position, const Vec2<TValue>& size) noexcept
			: _Position(position), _Size(size) {
		}
		template<typename UValue = TValue>
		constexpr Rect(const Rect<UValue>& rect) noexcept
			: _X(static_cast<TValue>(rect._X)), _Y(static_cast<TValue>(rect._Y)),
			_Width(static_cast<TValue>(rect._Width)), _Height(static_cast<TValue>(rect._Height)) {
		}
		template<typename UValue = TValue>
		constexpr Rect(const UValue& value) noexcept
			: _X(static_cast<TValue>(value)), _Y(static_cast<TValue>(value)),
			_Width(static_cast<TValue>(value)), _Height(static_cast<TValue>(value)) {
		}

		template<typename UValue = TValue>
		constexpr Rect(const sf::Rect<UValue>& rect) noexcept
			: _X(static_cast<TValue>(rect._X)), _Y(static_cast<TValue>(rect._Y)),
			_Width(static_cast<TValue>(rect._Width)), _Height(static_cast<TValue>(rect._Height)) {
		}
		template<typename UValue = TValue>
		constexpr operator sf::Rect<UValue>() const noexcept {
			return sf::Rect<UValue>(_Position, _Size);
		}

		constexpr TValue Area() const noexcept {
			return _Size.Area();
		}
		constexpr Vec2<float> Center() const noexcept {
			return static_cast<Vec2<float>>(_Position) + _Size.Center();
		}
		constexpr Vec2<TValue> Min() const noexcept {
			return _Position + Vec2<TValue>(std::min(_Size._X, 0), std::min(_Size._Y, 0));
		}
		constexpr Vec2<TValue> Max() const noexcept {
			return _Position + Vec2<TValue>(std::max(_Size._X, 0), std::max(_Size._Y, 0));
		}

		constexpr bool Contains(const Vec2<TValue>& vec) const noexcept {
			const Vec2<TValue> min = Min(), max = Max();
			return (vec._X >= min._X && vec._Y >= min._Y) && (vec._X <= max._X && vec._Y <= max._Y);
		}

		constexpr bool operator==(const Rect& rect) const noexcept {
			return _Position == rect._Position && _Size == rect._Size;
		}
		constexpr bool operator==(const Vec2<TValue>& vec) const noexcept {
			return _Position == vec && _Size == vec;
		}
		constexpr bool operator==(const TValue& value) const noexcept {
			return _Position == value && _Size == value;
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

		static constexpr Rect<TValue> Zero() noexcept {
			return Rect<TValue>(0);
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
			return std::format_to(context.out(), "{{{}, {}}}", rect._Position, rect._Size);
		}
	};
}
