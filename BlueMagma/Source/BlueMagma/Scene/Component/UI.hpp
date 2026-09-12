#pragma once
#include "Math/Vec2.hpp"
#include "Math/Color.hpp"
#include "Event/Event.hpp"
#include "Scene/Entity.hpp"

#include <functional>
#include <string>
#include <cstdint>
#include <cctype>

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

	//======================================================================================

	struct WidgetColor
	{
		Color IdleColor = ColorDef::White;
		Color HoverColor = ColorDef::White;
		Color FocusColor = ColorDef::White;
	};

	//======================================================================================

	struct Clickable
	{
		using OnClickFn = std::function<bool(Entity, EventHandle::MouseButtonPressed)>;

		OnClickFn OnClick;
	};

	//======================================================================================

	struct InputText
	{
		using PolicyFn = std::function<bool(char32_t)>;

		std::string Text = "";
		std::string Placeholder = "";

		size_t CursorIndex = 0;
		size_t MaxLength = SIZE_MAX;

		PolicyFn Policy{ isprint };

		Entity TextChild{};
		Entity CursorChild{};
	};
}
