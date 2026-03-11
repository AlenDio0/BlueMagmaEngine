#include "bmpch.hpp"
#include "UISystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Component/Base.hpp"
#include "Scene/Component/UI.hpp"
#include "Core/Rect.hpp"
#include "Core/Timer.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/String.hpp>

namespace BM
{
	using namespace Component;

	void UISystem::OnEvent(Scene& scene, Event& event) noexcept
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed, scene));
		dispatcher.Dispatch<BM::EventHandle::MouseMoved>(BM_EVENT_FN(OnMouseMoved, scene));
		dispatcher.Dispatch<BM::EventHandle::TextEntered>(BM_EVENT_FN(OnTextEntered, scene));
		dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed, scene));
	}

	void UISystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		UpdateColor(scene);
		UpdateInputText(scene);
	}

	static inline Vec2f PixelToCoords(Scene& scene, Vec2i point) noexcept {
		if (Renderer* renderer = scene.GetRenderer())
			return renderer->PixelToCoords(point);
		return point;
	}

	static inline bool Contains(Scene& scene, const Transform& transform, const Widget& widget, Vec2i point) noexcept {
		const Vec2f cPosition = transform.Position - (widget.Size * transform.Scale * transform.Origin);
		const Vec2f cSize = widget.Size * transform.Scale;

		const Vec2f cCoords = PixelToCoords(scene, point);
		return RectFloat(cPosition, cSize).Contains(cCoords);
	}

	static inline size_t CoordsToCursorIndex(InputText& input, float coordsX) noexcept {
		Entity text = input.TextChild;
		if (!text)
			return 0;

		auto textTransform = text.TryGet<Transform>();
		auto textRender = text.TryGet<TextRender>();
		if (!textTransform || !textRender || !textRender->FontPtr || input.Text.empty())
			return 0;

		const sf::String cUtfText = input.Text;

		float textWidth = 0.f;
		if (textTransform->Origin.X != 0.f)
		{
			char32_t previousChar = 0;
			for (char32_t currentChar : cUtfText)
			{
				textWidth += textRender->FontPtr->getGlyph(currentChar, textRender->CharSize, false).advance;
				if (previousChar != 0)
					textWidth += textRender->FontPtr->getKerning(previousChar, currentChar, textRender->CharSize);
				previousChar = currentChar;
			}
		}
		const float cMousePivotX = (coordsX - textTransform->Position.X) / textTransform->Scale.X;
		const float cOriginOffset = textWidth * textTransform->Origin.X;
		const float cRelativeMouseX = cMousePivotX + cOriginOffset;

		char32_t previousChar = 0;
		float textX = 0.f;
		for (size_t i = 0; i < cUtfText.getSize(); i++)
		{
			char32_t currentChar = cUtfText[i];
			float advance = textRender->FontPtr->getGlyph(currentChar, textRender->CharSize, false).advance;
			if (previousChar != 0)
				advance += textRender->FontPtr->getKerning(previousChar, currentChar, textRender->CharSize);
			previousChar = currentChar;

			if (cRelativeMouseX < textX + (advance / 2.f))
				return i;

			textX += advance;
		}

		return cUtfText.getSize();
	}

	bool UISystem::OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept
	{
		bool dispatched = false;

		{
			auto view = scene.GetRegistry().view<Transform, Widget>();
			for (auto [entity, transform, widget] : view.each())
			{
				bool onMouse = Contains(scene, transform, widget, mousePressed.position);
				widget.Focus = onMouse;
				widget.Hover = onMouse;
			}
		}

		{
			auto view = scene.GetRegistry().view<Widget, Clickable>();
			for (auto [entity, widget, clickable] : view.each())
			{
				const auto& onClick = clickable.OnClick;
				if (!onClick)
					continue;

				if (widget.Focus)
				{
					dispatched = onClick(Entity(&scene, entity), mousePressed);
					if (dispatched)
						return true;
				}
			}
		}
		{
			auto view = scene.GetRegistry().view<Widget, InputText>();
			for (auto [entity, widget, input] : view.each())
			{
				if (!widget.Focus)
					continue;
				dispatched = true;

				input.CursorIndex = CoordsToCursorIndex(input, PixelToCoords(scene, mousePressed.position).X);
				Entity cursor = input.CursorChild;
				if (cursor)
				{
					cursor.TryPatch<Hidden>([&](auto& hidden) {
						hidden.Visible = true;
						m_CursorBlinkTimer.Restart();
						});
				}
			}
		}

		return dispatched;
	}

	bool UISystem::OnMouseMoved(const EventHandle::MouseMoved& mouseMoved, Scene& scene) noexcept
	{
		auto view = scene.GetRegistry().view<Transform, Widget>();
		for (auto [entity, transform, widget] : view.each())
			widget.Hover = Contains(scene, transform, widget, mouseMoved.position);

		return false;
	}

	bool UISystem::OnTextEntered(const EventHandle::TextEntered& textEntered, Scene& scene) noexcept
	{
		bool dispatched = false;

		auto view = scene.GetRegistry().view<Widget, InputText>();
		for (auto [entity, widget, input] : view.each())
		{
			if (!widget.Focus)
				continue;
			dispatched = true;

			const char32_t cUnicodeInput = textEntered.unicode;

			auto& text = input.Text;
			auto& cursorIndex = input.CursorIndex;

			const bool cIsBackspaceKey = cUnicodeInput == SpecialKey::Backspace;
			if (cIsBackspaceKey)
				continue;

			const bool cIsOverMaxLimit = text.size() >= input.MaxLength;
			if (cIsOverMaxLimit)
				continue;

			const bool cIsAgainstPolicy = input.Policy && !input.Policy(cUnicodeInput);
			if (cIsAgainstPolicy)
				continue;

			text.insert(cursorIndex, 1, static_cast<char>(cUnicodeInput));
			cursorIndex++;

			Entity inputText(&scene, entity);
			Entity cursor = input.CursorChild;
			if (cursor)
			{
				cursor.TryPatch<Hidden>([&](auto& hidden) {
					hidden.Visible = true;
					m_CursorBlinkTimer.Restart();
					});
			}
		}

		return dispatched;
	}

	bool UISystem::OnKeyPressed(const EventHandle::KeyPressed& keyPressed, Scene& scene) noexcept
	{
		bool dispatched = false;

		auto view = scene.GetRegistry().view<Widget, InputText>();
		for (auto [entity, widget, input] : view.each())
		{
			if (!widget.Focus)
				continue;
			dispatched = true;

			auto& cursorIndex = input.CursorIndex;
			const size_t cTextSize = input.Text.size();

			const bool cIsControlPressed = keyPressed.control;

			bool resetBlink = false;
			switch (keyPressed.code)
			{
				using Key = sf::Keyboard::Key;

			case Key::Escape:
			case Key::Enter:
				widget.Focus = false;
				break;
			case Key::Backspace:
				if (cursorIndex == 0)
					break;

				input.Text.erase(cursorIndex - 1, 1);
				cursorIndex--;
				break;
			case Key::Delete:
				if (cursorIndex < cTextSize)
					input.Text.erase(cursorIndex, 1);
				break;
			case Key::Left:
				cursorIndex = cIsControlPressed ? 0 : (cursorIndex == 0 ? 0 : --cursorIndex);
				resetBlink = true;
				break;
			case Key::Home:
				cursorIndex = 0;
				resetBlink = true;
				break;

			case Key::Right:
				cursorIndex = cIsControlPressed ? cTextSize : (cursorIndex >= cTextSize ? cTextSize : ++cursorIndex);
				resetBlink = true;
				break;
			case Key::End:
				cursorIndex = cTextSize;
				resetBlink = true;
				break;

			default:
				break;
			}

			if (resetBlink)
			{
				Entity cursor = input.CursorChild;
				if (cursor)
				{
					cursor.TryPatch<Hidden>([&](auto& hidden) {
						hidden.Visible = true;
						m_CursorBlinkTimer.Restart();
						});
				}
			}
		}

		return dispatched;
	}

	void UISystem::UpdateColor(Scene& scene) noexcept
	{
		{
			auto view = scene.GetRegistry().view<Style, Widget, HoverColor>();
			for (auto [entity, style, widget, hover] : view.each())
				style.FillColor = widget.Hover ? hover.Color : hover.IdleColor;
		}
		{
			auto view = scene.GetRegistry().view<Style, Widget, FocusColor>();
			for (auto [entity, style, widget, focus] : view.each())
				style.FillColor = widget.Focus ? focus.Color : focus.IdleColor;
		}
	}

	void UISystem::UpdateInputText(Scene& scene) noexcept
	{
		auto view = scene.GetRegistry().view<Transform, Widget, InputText>();
		for (auto [entity, transform, widget, input] : view.each())
		{
			Entity inputText(&scene, entity);

			Entity text = input.TextChild;
			if (text)
			{
				text.TryPatch<TextRender>([&](auto& textRender) {
					textRender.Text = !input.Text.empty() ? input.Text : input.Placeholder;
					});
				text.TryPatch<Style>([&](auto& style) {
					style.FillColor.a = !input.Text.empty() ? 0xFF : 0x80;
					});
			}

			Entity cursor = input.CursorChild;
			if (cursor)
			{
				cursor.TryPatch<Transform>([&](auto& transform) {
					const auto& textTransform = text.Get<Transform>();
					const auto& textRender = text.Get<TextRender>();
					if (!textRender.FontPtr)
						return;

					float textWidth = 0.f;
					if (textTransform.Origin.X != 0.f)
					{
						const sf::String cUtfString = input.Text;
						char32_t previousChar = 0;
						for (char32_t currentChar : cUtfString)
						{
							textWidth += textRender.FontPtr->getGlyph(currentChar, textRender.CharSize, false).advance;
							if (previousChar != 0)
								textWidth += textRender.FontPtr->getKerning(previousChar, currentChar, textRender.CharSize);
							previousChar = currentChar;
						}
					}

					sf::String textToCursor = input.Text.substr(0, input.CursorIndex);
					char32_t previousChar = 0;
					float offsetX = 0.f;
					for (char32_t currentChar : textToCursor)
					{
						offsetX += textRender.FontPtr->getGlyph(currentChar, textRender.CharSize, false).advance;
						if (previousChar != 0)
							offsetX += textRender.FontPtr->getKerning(previousChar, currentChar, textRender.CharSize);
						previousChar = currentChar;
					}

					const float cOriginOffsetX = textWidth * textTransform.Origin.X;
					transform.LocalPosition.X = std::round(textTransform.LocalPosition.X + offsetX - cOriginOffsetX) - 1.f;
					});
				cursor.TryPatch<Hidden>([&](auto& hidden) {
					if (!widget.Focus)
					{
						hidden.Visible = false;
						return;
					}

					if (m_CursorBlinkTimer.AsSeconds() >= 0.5f)
					{
						hidden.Visible = !hidden.Visible;
						m_CursorBlinkTimer.Restart();
					}
					});
			}
		}
	}
}
