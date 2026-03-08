#pragma once
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "Base.hpp"
#include "UI.hpp"
#include "Core/Vec2.hpp"

namespace BM::UIMaker
{
	struct UIProps
	{
		Component::Transform Transform;
		Vec2f Size;
		float Corner;
		Component::Style Style;
	};
	struct TextProps
	{
		Component::TextRender Text;
		Component::Style Style;
		bool Centered = false;
	};

	Entity AddTextChild(Entity entity, const TextProps& props) noexcept;
	void AddHoverColor(Entity entity, float factor = 0.75f) noexcept;

	Entity CreateUI(Scene& scene, const UIProps& props) noexcept;

	Entity CreateButton(Scene& scene, const UIProps& props, Component::ClickFn onClick = nullptr) noexcept;

	Entity CreateInputText(Scene& scene, const UIProps& baseProps, const TextProps& textProps, Component::InputText input) noexcept;
}
