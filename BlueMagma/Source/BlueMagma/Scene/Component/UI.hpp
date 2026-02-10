#pragma once
#include "Scene/Entity.hpp"
#include "Core/Vec2.hpp"
#include "Base/EventDispatcher.hpp"
#include <functional>
#include <sstream>
#include <string>

namespace BM::Component
{
	struct Widget
	{
		Vec2f _Size{};

		bool _Focus = false;
		bool _Hover = false;
	};


	struct HoverColor
	{
		sf::Color _IdleColor = sf::Color::White;
		sf::Color _HoverColor = sf::Color(0xFFFFFFA0);
	};

	struct FocusColor
	{
		sf::Color _IdleColor = sf::Color::White;
		sf::Color _FocusColor = sf::Color(0xFFFFFFA0);
	};

	using ClickFn = std::function<bool(Entity, EventHandle::MouseButtonPressed)>;
	struct Clickable
	{
		ClickFn _OnClick;
	};

	using PolicyFn = std::function<bool(int)>;
	struct InputText
	{
		std::ostringstream _Buffer{ "" };
		std::string _Placeholder;

		PolicyFn _Policy;
		size_t _MaxLength;

		inline InputText(std::string_view placeholder = "", size_t maxLength = -1, const PolicyFn& policy = nullptr) noexcept
			: _Placeholder(placeholder), _Policy(policy), _MaxLength(maxLength) {
		}
	};
}
