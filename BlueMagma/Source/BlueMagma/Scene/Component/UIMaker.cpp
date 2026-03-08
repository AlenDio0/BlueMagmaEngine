#include "bmpch.hpp"
#include "UIMaker.hpp"

namespace BM::UIMaker
{
	using namespace Component;

	Entity AddTextChild(Entity entity, const TextProps& props) noexcept
	{
		BM_CORE_FN("entity: {}", entity);
		auto transform = entity.TryGet<Transform>();
		auto widget = entity.TryGet<Widget>();
		if (!transform || !widget)
		{
			BM_CORE_WARN("Cannot add a Text child without Transform and Widget [entity: {}]", entity);
			return Entity();
		}

		const Vec2f cSize = widget->Size;
		const Vec2f cCenterPosition = (cSize.Center() - (cSize * transform->Origin)).Round();

		const Vec2f cPosition = Vec2f(props.Centered ? cCenterPosition.X : 5.f, cCenterPosition.Y);
		const Vec2f cOrigin = Vec2f(props.Centered ? 0.5f : 0.f, 0.5f);

		Entity text = entity.CreateChild(Transform(cPosition, 1.f, 1.f, cOrigin));

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
		Entity ui = scene.Create(props.Transform);
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

		Transform cursorTransform = text.Get<Transform>();
		cursorTransform.Origin = Vec2f(0.f, 0.5f);
		cursorTransform.LocalZ += 1.f;

		Entity cursor = inputText.CreateChild(cursorTransform);
		cursor.Add<RectRender>(Vec2f(2.f, static_cast<float>(textProps.Text.CharSize) * 1.1f));
		cursor.Add<Style>(textProps.Style);
		cursor.Add<Hidden>();

		input.TextChild = text;
		input.CursorChild = cursor;
		inputText.Add<InputText>(std::move(input));

		BM_CORE_DEBUG("Created InputText [entity: {}]", inputText);
		return inputText;
	}
}
