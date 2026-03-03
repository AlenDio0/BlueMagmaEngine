#include "bmpch.hpp"
#include "UISystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Component/Base.hpp"
#include "Scene/Component/UI.hpp"
#include "Core/Rect.hpp"

namespace BM
{
	void UISystem::OnEvent(Scene& scene, Event& event) noexcept
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed, scene));
		dispatcher.Dispatch<BM::EventHandle::MouseMoved>(BM_EVENT_FN(OnMouseMoved, scene));
		dispatcher.Dispatch<BM::EventHandle::TextEntered>(BM_EVENT_FN(OnTextEntered, scene));
	}

	void UISystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		UpdateColor(scene);
		UpdateInputText(scene);
	}

	static inline bool Contains(Scene& scene, const Component::Transform& transform, const Component::Widget& widget, Vec2i point) noexcept {
		if (Renderer* renderer = scene.GetRenderer())
			point = renderer->PixelToCoords(point);

		const Vec2f cPosition = transform.Position - (widget.Size * transform.Scale * transform.Origin);
		const Vec2f cSize = widget.Size * transform.Scale;

		return RectFloat(cPosition, cSize).Contains(point);
	}

	bool UISystem::OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept
	{
		{
			auto view = scene.GetRegistry().view<Component::Transform, Component::Widget>();
			for (auto [entity, transform, widget] : view.each())
			{
				bool onMouse = Contains(scene, transform, widget, mousePressed.position);
				widget.Focus = onMouse;
				widget.Hover = onMouse;
			}
		}
		{
			auto view = scene.GetRegistry().view<Component::Widget, Component::Clickable>();
			for (auto [entity, widget, clickable] : view.each())
			{
				auto& onClick = clickable.OnClick;
				if (!onClick)
					continue;

				if (widget.Focus)
				{
					bool dispatched = onClick(Entity(&scene, entity), mousePressed);
					if (dispatched)
						return true;
				}
			}
		}

		return false;
	}

	bool UISystem::OnMouseMoved(const EventHandle::MouseMoved& mouseMoved, Scene& scene) noexcept
	{
		auto view = scene.GetRegistry().view<Component::Transform, Component::Widget>();
		for (auto [entity, transform, widget] : view.each())
			widget.Hover = Contains(scene, transform, widget, mouseMoved.position);

		return false;
	}

	bool UISystem::OnTextEntered(const EventHandle::TextEntered& textEntered, Scene& scene) noexcept
	{
		auto view = scene.GetRegistry().view<Component::Widget, Component::InputText>();
		for (auto [entity, widget, inputText] : view.each())
		{
			if (!widget.Focus)
				continue;

			uint32_t input = textEntered.unicode;
			auto& buff = inputText.Buffer;

			const std::string cBuffStr = buff.str();
			const size_t cSize = cBuffStr.size();

			const bool cDelete = input == SpecialKey::Delete;

			if (cSize >= inputText.MaxLength && !cDelete)
				continue;
			if (inputText.Policy && !inputText.Policy(input))
				continue;

			switch (input)
			{
			case SpecialKey::Delete:
				buff.str("");
				buff << cBuffStr.substr(0, cSize - 1);
				break;
			case SpecialKey::Escape:
			case SpecialKey::Enter:
				widget.Focus = false;
				break;
			default:
				buff << (char)input;
				break;
			}
		}

		return true;
	}

	void UISystem::UpdateColor(Scene& scene) noexcept
	{
		{
			auto view = scene.GetRegistry().view<Component::Style, Component::Widget, Component::HoverColor>();
			for (auto [entity, style, widget, hover] : view.each())
				style.FillColor = widget.Hover ? hover.Color : hover.IdleColor;
		}
		{
			auto view = scene.GetRegistry().view<Component::Style, Component::Widget, Component::FocusColor>();
			for (auto [entity, style, widget, focus] : view.each())
				style.FillColor = widget.Focus ? focus.Color : focus.IdleColor;
		}
	}

	void UISystem::UpdateInputText(Scene& scene) noexcept
	{
		auto view = scene.GetRegistry().view<Component::Widget, Component::InputText>();
		for (auto [entity, widget, input] : view.each())
		{
			Entity inputText(&scene, entity);

			auto children = inputText.GetChildren<Component::Transform, Component::TextRender>();
			for (auto& child : children)
			{
				child.Patch<Component::TextRender>([&](auto& textRender) {
					textRender.Text = !input.Buffer.str().empty() ? input.Buffer.str() : input.Placeholder;
					});
			}
		}
	}
}
