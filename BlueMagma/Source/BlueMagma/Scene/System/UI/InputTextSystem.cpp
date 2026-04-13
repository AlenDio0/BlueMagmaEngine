#include "bmpch.hpp"
#include "InputTextSystem.hpp"
#include "Scene/Component/Base.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/String.hpp>

namespace BM::UI
{
	using namespace Component;

	void InputTextSystem::OnAttach(Scene& scene) noexcept
	{
		scene.OnConstruct<InputText>().connect<&InputTextSystem::UpdateInputText>(this);
		scene.OnUpdate<InputText>().connect<&InputTextSystem::UpdateInputText>(this);

		scene.OnUpdate<Widget>().connect<&InputTextSystem::UpdateCursorHidden>(this);
	}

	void InputTextSystem::OnEvent(Scene& scene, Event& event) noexcept
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed, scene));
		dispatcher.Dispatch<EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed, scene));
		dispatcher.Dispatch<EventHandle::TextEntered>(BM_EVENT_FN(OnTextEntered, scene));
	}

	void InputTextSystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		auto view = scene.View<Transform, Widget, InputText>();
		for (auto [entity, transform, widget, input] : view.each())
		{
			if (!widget.Focus)
				continue;

			Entity cursor = input.CursorChild;
			if (cursor && m_CursorBlinkTimer.AsSeconds() >= 0.5f)
			{
				cursor.TryPatch<Hidden>([&](auto& hidden) {
					hidden.Visible = !hidden.Visible; });
				m_CursorBlinkTimer.Restart();
			}
		}
	}

	float InputTextSystem::CursorIndexCoords(const Component::InputText& input) noexcept
	{
		Entity text = input.TextChild;
		const auto* textTransform = text.TryGet<Transform>();
		const auto* textRender = text.TryGet<TextRender>();
		if (!textTransform || !textRender)
			return 0.f;

		if (!textRender->FontPtr)
			return 0.f;

		float textWidth = 0.f;
		if (textTransform->Local.State.Origin.X != 0.f)
		{
			const sf::String cUtfString = input.Text;
			char32_t previousChar = 0;
			for (char32_t currentChar : cUtfString)
			{
				textWidth += textRender->FontPtr->getGlyph(currentChar, textRender->CharSize, false).advance;
				if (previousChar != 0)
					textWidth += textRender->FontPtr->getKerning(previousChar, currentChar, textRender->CharSize);
				previousChar = currentChar;
			}
		}

		sf::String textToCursor = input.Text.substr(0, input.CursorIndex);
		char32_t previousChar = 0;
		float offsetX = 0.f;
		for (char32_t currentChar : textToCursor)
		{
			offsetX += textRender->FontPtr->getGlyph(currentChar, textRender->CharSize, false).advance;
			if (previousChar != 0)
				offsetX += textRender->FontPtr->getKerning(previousChar, currentChar, textRender->CharSize);
			previousChar = currentChar;
		}

		const float cOriginOffsetX = textWidth * textTransform->Local.State.Origin.X;
		return std::round(textTransform->Local.State.Position.X + offsetX - cOriginOffsetX) - 1.f;
	}

	size_t InputTextSystem::CoordsToCursorIndex(const InputText& input, float coordsX) noexcept
	{
		Entity text = input.TextChild;
		if (!text)
			return 0;

		auto textTransform = text.TryGet<Transform>();
		auto textRender = text.TryGet<TextRender>();
		if (!textTransform || !textRender || !textRender->FontPtr || input.Text.empty())
			return 0;

		const sf::String cUtfText = input.Text;

		float textWidth = 0.f;
		if (textTransform->Local.State.Origin.X != 0.f)
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
		const float cMousePivotX = (coordsX - textTransform->Global.Position.X) / textTransform->Global.Scale.X;
		const float cOriginOffset = textWidth * textTransform->Local.State.Origin.X;
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

	void InputTextSystem::UpdateInputText(Registry& registry, EntityHandle entity) noexcept
	{
		const auto& input = registry.get<InputText>(entity);
		Entity text = input.TextChild;
		Entity cursor = input.CursorChild;

		if (text)
		{
			text.TryPatch<TextRender>([&](auto& textRender) {
				textRender.Text = !input.Text.empty() ? input.Text : input.Placeholder; });
			text.TryPatch<Style>([&](auto& style) {
				style.FillColor.a = !input.Text.empty() ? 0xFF : 0x80; });
		}

		if (cursor)
		{
			cursor.TryPatch<Transform>([&](auto& transform) {
				transform.Local.State.Position.X = CursorIndexCoords(input);
				});
		}
	}

	void InputTextSystem::UpdateCursorHidden(Registry& registry, EntityHandle entity) noexcept
	{
		const auto& widget = registry.get<Widget>(entity);
		const auto input = registry.try_get<InputText>(entity);
		if (!input)
			return;

		Entity cursor = input->CursorChild;
		if (cursor)
		{
			cursor.TryPatch<Hidden>([&](auto& hidden) {
				if (!widget.Focus)
					hidden.Visible = false;
				});
		}
	}

	bool InputTextSystem::OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept
	{
		bool dispatched = false;

		auto view = scene.View<Widget, InputText>();
		for (auto [entity, widget, input] : view.each())
		{
			if (!widget.Focus)
				continue;
			dispatched = true;

			Vec2f coords = mousePressed.position;
			if (auto renderer = scene.GetRenderer())
				coords = renderer->PixelToCoords(mousePressed.position);

			scene.PatchComponent<InputText>(entity, [&](auto& input) {
				input.CursorIndex = CoordsToCursorIndex(input, coords.X); });

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

	bool InputTextSystem::OnKeyPressed(const EventHandle::KeyPressed& keyPressed, Scene& scene) noexcept
	{
		bool dispatched = false;

		auto view = scene.View<Widget, InputText>();
		for (auto [entity, widget, input] : view.each())
		{
			if (!widget.Focus)
				continue;
			dispatched = true;

			Entity inputText = scene.GetEntity(entity);

			const size_t& cursorIndex = input.CursorIndex;
			const size_t cTextSize = input.Text.size();

			const bool cIsControlPressed = keyPressed.control;

			bool resetBlink = false;
			switch (keyPressed.code)
			{
				using Key = sf::Keyboard::Key;

			case Key::Escape:
			case Key::Enter:
				inputText.Patch<Widget>([](auto& widget) { widget.Focus = false; });
				break;
			case Key::Backspace:
				if (cursorIndex == 0)
					break;

				inputText.Patch<InputText>([&](InputText& input) {
					if (!cIsControlPressed)
					{
						input.Text.erase(cursorIndex - 1, 1);
						input.CursorIndex--;
					}
					else
					{
						input.Text.erase(0, cursorIndex);
						input.CursorIndex = 0;
					}
					});
				break;
			case Key::Delete:
				if (cursorIndex < cTextSize)
					inputText.Patch<InputText>([&](auto& input) { input.Text.erase(cursorIndex, 1); });
				break;
			case Key::Left:
				inputText.Patch<InputText>([&](auto& input) {
					input.CursorIndex = cIsControlPressed ? 0 : std::max<size_t>(cursorIndex - 1ull, 0ull); });
				resetBlink = true;
				break;
			case Key::Home:
				inputText.Patch<InputText>([&](auto& input) { input.CursorIndex = 0; });
				resetBlink = true;
				break;

			case Key::Right:
				inputText.Patch<InputText>([&](auto& input) {
					input.CursorIndex = cIsControlPressed ? cTextSize : std::min<size_t>(cursorIndex + 1ull, cTextSize); });
				resetBlink = true;
				break;
			case Key::End:
				inputText.Patch<InputText>([&](auto& input) { input.CursorIndex = cTextSize; });
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

	bool InputTextSystem::OnTextEntered(const EventHandle::TextEntered& textEntered, Scene& scene) noexcept
	{
		bool dispatched = false;

		auto view = scene.GetRegistry().view<Widget, InputText>();
		for (auto [entity, widget, input] : view.each())
		{
			if (!widget.Focus)
				continue;
			dispatched = true;

			const char32_t cUnicodeInput = textEntered.unicode;
			const size_t& cursorIndex = input.CursorIndex;

			const bool cIsBackspaceKey = cUnicodeInput == SpecialKey::Backspace;
			if (cIsBackspaceKey)
				continue;

			const bool cIsOverMaxLimit = input.Text.size() >= input.MaxLength;
			if (cIsOverMaxLimit)
				continue;

			const bool cIsAgainstPolicy = input.Policy && !input.Policy(cUnicodeInput);
			if (cIsAgainstPolicy)
				continue;

			scene.PatchComponent<InputText>(entity, [&](auto& input) {
				input.Text.insert(cursorIndex, 1, static_cast<char>(cUnicodeInput));
				input.CursorIndex++;
				});

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
}
