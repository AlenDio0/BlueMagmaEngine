#include "bmpch.hpp"
#include "UIMaker.hpp"

namespace BM::UIMaker
{
	using namespace Component;

	void AddTextChild(Entity entity, TextRender textRender, Style style) noexcept
	{
		BM_CORE_FN("entity: {}", entity);
		auto transform = entity.TryGet<Transform>();
		auto widget = entity.TryGet<Widget>();
		if (!transform || !widget)
		{
			BM_CORE_WARN("Cannot add TextChild without Transform and Widget [entity: {}]", entity);
			return;
		}

		const Vec2f cOrigin = transform->Origin;
		const Vec2f cSize = widget->Size;

		Entity text = entity.CreateChild(Transform(cSize.Center() - (cSize * cOrigin), 1.f, 1.f, 0.5f));

		text.Add<TextRender>(std::move(textRender));
		text.Add<Style>(std::move(style));
	}

	void AddHoverColor(Entity entity, float factor) noexcept
	{
		BM_CORE_FN("entity: {}", entity);
		auto style = entity.TryGet<Style>();
		if (!style)
		{
			BM_CORE_WARN("Cannot add HoverColor without Style [entity: {}]", entity);
			return;
		}

		sf::Color hoverColor = style->FillColor;
		hoverColor.r = static_cast<uint32_t>(hoverColor.r * factor);
		hoverColor.g = static_cast<uint32_t>(hoverColor.g * factor);
		hoverColor.b = static_cast<uint32_t>(hoverColor.b * factor);

		entity.Add<HoverColor>(style->FillColor, hoverColor);
	}

	Entity CreateUI(Scene& scene, Component::Transform transform, Vec2f size, float corner, Component::Style style) noexcept
	{
		BM_CORE_FN();
		Entity ui = scene.Create(transform);
		ui.Add<RectRender>(size, corner);
		ui.Add<Style>(style);
		ui.Add<Widget>(size);

		BM_CORE_TRACE("Created UI [entity: {}]", ui);
		return ui;
	}

	Entity CreateButton(Scene& scene, Transform transform, Vec2f size, float corner, Style buttonStyle, ClickFn onClick) noexcept
	{
		Entity button = CreateUI(scene, transform, size, corner, buttonStyle);
		button.Add<Clickable>(onClick);

		BM_CORE_DEBUG("Created Button [entity: {}]", button);
		return button;
	}

	Entity CreateInputText(Scene& scene, Transform transform, Vec2f size, float corner, Style backgroundStyle,
		TextRender textRender, Style textStyle, InputText input) noexcept
	{
		Entity inputText = CreateUI(scene, transform, size, corner, backgroundStyle);
		inputText.Add<InputText>(std::move(input));

		AddTextChild(inputText, textRender, textStyle);

		BM_CORE_DEBUG("Created InputText [entity: {}]", inputText);
		return inputText;
	}
}
