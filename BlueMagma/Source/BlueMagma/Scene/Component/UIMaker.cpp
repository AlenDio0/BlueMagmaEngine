#include "bmpch.hpp"
#include "UIMaker.hpp"

namespace BM::UIMaker
{
	using namespace Component;

	void AddTextChild(Entity entity, TextRender textRender, Style style) noexcept
	{
		auto transform = entity.TryGet<Transform>();
		auto widget = entity.TryGet<Widget>();
		if (!widget || !transform)
			return;

		const Vec2f cOrigin = transform->_Origin;
		const Vec2f cSize = widget->_Size;

		Entity text = entity.CreateChild(Transform(cSize.Center() - (cSize * cOrigin), 1.f, 1.f, 0.5f));

		text.Add<TextRender>(std::move(textRender));
		text.Add<Style>(std::move(style));
	}

	void AddHoverColor(Entity entity, float factor) noexcept
	{
		auto style = entity.TryGet<Style>();
		if (!style)
			return;

		sf::Color hoverColor = style->_FillColor;
		hoverColor.r = static_cast<uint32_t>(hoverColor.r * factor);
		hoverColor.g = static_cast<uint32_t>(hoverColor.g * factor);
		hoverColor.b = static_cast<uint32_t>(hoverColor.b * factor);

		entity.Add<HoverColor>(style->_FillColor, hoverColor);
	}

	Entity CreateUI(Scene& scene, Component::Transform transform, Vec2f size, float corner, Component::Style style) noexcept
	{
		Entity ui = scene.Create(transform);
		ui.Add<RectRender>(size, corner);
		ui.Add<Style>(style);
		ui.Add<Widget>(size);

		return ui;
	}

	Entity CreateButton(Scene& scene, Transform transform, Vec2f size, float corner, Style buttonStyle, ClickFn onClick) noexcept
	{
		Entity button = CreateUI(scene, transform, size, corner, buttonStyle);
		button.Add<Clickable>(onClick);

		return button;
	}

	Entity CreateInputText(Scene& scene, Transform transform, Vec2f size, float corner, Style backgroundStyle,
		TextRender textRender, Style textStyle, InputText input) noexcept
	{
		Entity inputText = CreateUI(scene, transform, size, corner, backgroundStyle);
		inputText.Add<InputText>(std::move(input));

		AddTextChild(inputText, textRender, textStyle);

		return inputText;
	}
}
