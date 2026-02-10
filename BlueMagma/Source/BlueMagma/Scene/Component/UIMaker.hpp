#pragma once
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "Base.hpp"
#include "UI.hpp"
#include "Core/Vec2.hpp"

namespace BM::UIMaker
{
	void AddTextChild(Entity entity, Component::TextRender textRender, Component::Style style) noexcept;
	void AddHoverColor(Entity entity, float factor = 0.75f) noexcept;

	Entity CreateUI(Scene& scene, Component::Transform transform, Vec2f size, float corner, Component::Style style) noexcept;

	Entity CreateButton(Scene& scene, Component::Transform transform, Vec2f size, float corner, Component::Style buttonStyle, Component::ClickFn onClick = nullptr) noexcept;

	Entity CreateInputText(Scene& scene, Component::Transform transform, Vec2f size, float corner, Component::Style backgroundStyle,
		Component::TextRender textRender, Component::Style textStyle, Component::InputText input) noexcept;
}
