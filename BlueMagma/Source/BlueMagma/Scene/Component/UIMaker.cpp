#include "bmpch.hpp"
#include "UIMaker.hpp"

namespace BM::UIMaker
{
	using namespace Component;

	Vec2f Center(Vec2f size, Vec2f origin) noexcept
	{
		const Vec2f cTopLeft = (size * origin) * -1.f;
		return cTopLeft + size.Center();
	}

	Entity AddTextChild(Entity entity, const TextProps& props) noexcept
	{
		BM_CORE_FN("entity: {}", entity);

		Entity text = entity.CreateChild(props.Transform);
		text.Add<TextRender>(props.Text);
		text.Add<Style>(props.Style);

		return text;
	}

	void AddHoverColor(Entity entity, float factor) noexcept
	{
		BM_CORE_FN("entity: {}", entity);
		auto style = entity.TryGet<Style>();
		if (!style)
		{
			BM_CORE_WARN("Cannot add an HoverColor without Style [entity: {}]", entity);
			return;
		}

		sf::Color hoverColor = style->FillColor;
		hoverColor.r = static_cast<uint32_t>(hoverColor.r * factor);
		hoverColor.g = static_cast<uint32_t>(hoverColor.g * factor);
		hoverColor.b = static_cast<uint32_t>(hoverColor.b * factor);

		entity.Add<HoverColor>(style->FillColor, hoverColor);
	}

	Entity CreateUI(Scene& scene, const UIProps& props) noexcept
	{
		BM_CORE_FN();
		Entity ui = scene.CreateEntity(props.Transform);
		ui.Add<RectRender>(props.Size, props.Corner);
		ui.Add<Style>(props.Style);
		ui.Add<Widget>(props.Size);

		BM_CORE_TRACE("Created UI [entity: {}]", ui);
		return ui;
	}

	Entity CreateButton(Scene& scene, const UIProps& props, ClickFn onClick) noexcept
	{
		Entity button = CreateUI(scene, props);
		button.Add<Clickable>(onClick);

		BM_CORE_DEBUG("Created Button [entity: {}]", button);
		return button;
	}

	Entity CreateInputText(Scene& scene, const UIProps& baseProps, const TextProps& textProps, Component::InputText input) noexcept
	{
		Entity inputText = CreateUI(scene, baseProps);
		Entity text = AddTextChild(inputText, textProps);

		Transform cursorTransform = textProps.Transform;
		cursorTransform.Origin = 0.5f;
		cursorTransform.LocalZ += 1.f;

		Entity cursor = inputText.CreateChild(cursorTransform);
		cursor.Add<RectRender>(Vec2f(1.f, static_cast<float>(textProps.Text.CharSize) * 1.1f));
		cursor.Add<Style>(textProps.Style);
		cursor.Add<Hidden>();

		input.TextChild = text;
		input.CursorChild = cursor;
		inputText.Add<InputText>(std::move(input));

		BM_CORE_DEBUG("Created InputText [entity: {}]", inputText);
		return inputText;
	}
}
