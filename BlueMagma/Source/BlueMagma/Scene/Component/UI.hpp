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
		Vec2f Size{};

		bool Focus = false;
		bool Hover = false;
	};


	struct HoverColor
	{
		sf::Color IdleColor = sf::Color::White;
		sf::Color Color = sf::Color(0xFFFFFFA0);
	};

	struct FocusColor
	{
		sf::Color IdleColor = sf::Color::White;
		sf::Color Color = sf::Color(0xFFFFFFA0);
	};

	using ClickFn = std::function<bool(Entity, EventHandle::MouseButtonPressed)>;
	struct Clickable
	{
		ClickFn OnClick;
	};

	using PolicyFn = std::function<bool(int)>;
	struct InputText
	{
		std::ostringstream Buffer{ "" };
		std::string Placeholder;

		PolicyFn Policy;
		size_t MaxLength;

		inline InputText(std::string_view placeholder = "", size_t maxLength = -1, const PolicyFn& policy = nullptr) noexcept
			: Placeholder(placeholder), Policy(policy), MaxLength(maxLength) {
		}
	};
}
