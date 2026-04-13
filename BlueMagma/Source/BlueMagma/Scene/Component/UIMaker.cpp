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

	void AddWidgetColor(Entity entity, float hoverFactor, float focusFactor) noexcept
	{
		BM_CORE_FN("entity: {}", entity);
		auto style = entity.TryGet<Style>();
		if (!style)
		{
			BM_CORE_WARN("Cannot calculate Component::WidgetColor factor without Component::Style [entity: {}]", entity);
			return;
		}

		sf::Color hoverColor = style->FillColor;
		hoverColor.r = static_cast<uint8_t>(hoverColor.r * hoverFactor);
		hoverColor.g = static_cast<uint8_t>(hoverColor.g * hoverFactor);
		hoverColor.b = static_cast<uint8_t>(hoverColor.b * hoverFactor);

		sf::Color focusColor = style->FillColor;
		focusColor.r = static_cast<uint8_t>(focusColor.r * focusFactor);
		focusColor.g = static_cast<uint8_t>(focusColor.g * focusFactor);
		focusColor.b = static_cast<uint8_t>(focusColor.b * focusFactor);

		auto& widgetColor = entity.AddOrGet<WidgetColor>(style->FillColor, hoverColor, focusColor);
		widgetColor.HoverColor = hoverColor;
		widgetColor.FocusColor = focusColor;
	}

	Entity CreateUI(Scene& scene, const UIProps& props) noexcept
	{
		BM_CORE_FN();
		Entity ui = scene.CreateEntity(props.Transform);
		ui.Add<Widget>(props.Size, props.Shape);
		ui.Add<Style>(props.Style);

		switch (props.Shape)
		{
			using ShapeType = Widget::ShapeType;

		case ShapeType::Rect:
			ui.Add<RectRender>(props.Size, props.Corner);
			break;
		case ShapeType::Circle:
			ui.Add<CircleRender>(props.Size.X / 2.f);
		}

		BM_CORE_TRACE("Created UI [ui: {}]", ui);
		return ui;
	}

	Entity CreateButton(Scene& scene, const UIProps& props, const ClickFn& onClick) noexcept
	{
		Entity button = CreateUI(scene, props);
		button.Add<Clickable>(onClick);

		BM_CORE_DEBUG("Created Button [button: {}]", button);
		return button;
	}

	Entity CreateInputText(Scene& scene, const UIProps& baseProps, const TextProps& textProps, InputText input) noexcept
	{
		Entity inputText = CreateUI(scene, baseProps);
		Entity text = AddTextChild(inputText, textProps);

		auto cursorTransform = textProps.Transform;
		cursorTransform.State.Origin = 0.5f;
		cursorTransform.Z += 1.f;

		Entity cursor = inputText.CreateChild(cursorTransform);
		cursor.Add<RectRender>(Vec2f(2.f, static_cast<float>(textProps.Text.CharSize) * 1.1f));
		cursor.Add<Style>(textProps.Style);
		cursor.Add<Hidden>();

		input.TextChild = text;
		input.CursorChild = cursor;
		inputText.Add<InputText>(std::move(input));

		BM_CORE_DEBUG("Created InputText [input: {}, text: {}, cursor: {}]", inputText, text, cursor);
		return inputText;
	}
}
