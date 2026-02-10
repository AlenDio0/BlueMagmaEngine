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

	static inline bool Contains(const Component::Transform& transform, const Component::Widget& widget, Vec2f point) noexcept {
		const Vec2f cPosition = transform._Position - (widget._Size * transform._Scale * transform._Origin);
		const Vec2f cSize = widget._Size * transform._Scale;

		return RectFloat(cPosition, cSize).Contains(point);
	}

	bool UISystem::OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept
	{
		{
			auto view = scene.GetRegistry().view<Component::Transform, Component::Widget>();
			for (auto [entity, transform, widget] : view.each())
			{
				bool onMouse = Contains(transform, widget, mousePressed.position);
				widget._Focus = onMouse;
				widget._Hover = onMouse;
			}
		}
		{
			auto view = scene.GetRegistry().view<Component::Widget, Component::Clickable>();
			for (auto [entity, widget, clickable] : view.each())
			{
				auto& onClick = clickable._OnClick;
				if (!onClick)
					continue;

				if (widget._Focus)
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
			widget._Hover = Contains(transform, widget, mouseMoved.position);

		return false;
	}

	bool UISystem::OnTextEntered(const EventHandle::TextEntered& textEntered, Scene& scene) noexcept
	{
		auto view = scene.GetRegistry().view<Component::Widget, Component::InputText>();
		for (auto [entity, widget, inputText] : view.each())
		{
			if (!widget._Focus)
				continue;

			uint32_t input = textEntered.unicode;
			auto& buff = inputText._Buffer;

			const std::string cBuffStr = buff.str();
			const size_t cSize = cBuffStr.size();

			const bool cDelete = input == SpecialKey::Delete;

			if (cSize >= inputText._MaxLength && !cDelete)
				continue;
			if (inputText._Policy && !inputText._Policy(input))
				continue;

			switch (input)
			{
			case SpecialKey::Delete:
				buff.str("");
				buff << cBuffStr.substr(0, cSize - 1);
				break;
			case SpecialKey::Escape:
			case SpecialKey::Enter:
				widget._Focus = false;
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
				style._FillColor = widget._Hover ? hover._HoverColor : hover._IdleColor;
		}
		{
			auto view = scene.GetRegistry().view<Component::Style, Component::Widget, Component::FocusColor>();
			for (auto [entity, style, widget, focus] : view.each())
				style._FillColor = widget._Focus ? focus._FocusColor : focus._IdleColor;
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
					textRender._Content = !input._Buffer.str().empty() ? input._Buffer.str() : input._Placeholder;
					});
			}
		}
	}
}
