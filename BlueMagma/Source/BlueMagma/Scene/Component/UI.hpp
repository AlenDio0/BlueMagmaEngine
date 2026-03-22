#pragma once
#include "Scene/Entity.hpp"
#include "Core/Vec2.hpp"
#include "Base/EventDispatcher.hpp"
#include <functional>
#include <string>
#include <cstdint>

namespace BM::Component
{
	struct Widget
	{
		enum ShapeType : uint8_t { Rect, Circle };

		Vec2f Size{};
		ShapeType Shape = ShapeType::Rect;

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

	using PolicyFn = std::function<bool(char32_t)>;
	struct InputText
	{
		std::string Text = "";
		std::string Placeholder = "";

		size_t CursorIndex = 0;
		size_t MaxLength = 0xFFFFFFFFFFFFFFFFull;

		PolicyFn Policy;

		Entity TextChild{};
		Entity CursorChild{};
	};
}
