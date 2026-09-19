#include "bmpch.hpp"
#include "ClickableSystem.hpp"

#include "Event/EventDispatcher.hpp"
#include "Scene/Component/UI.hpp"

namespace BM::UI
{
	using namespace Component;

	void ClickableSystem::OnEvent(Scene& scene, Event& event) noexcept
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed, scene));
	}

	bool ClickableSystem::OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept
	{
		auto view = scene.View<Widget, Clickable>();
		for (auto [entity, widget, clickable] : view.each())
		{
			const auto& onClick = clickable.OnClick;
			if (!widget.Focus)
				continue;

			bool dispatched = false;

			if (onClick)
				dispatched = onClick(scene.GetEntity(entity), mousePressed);

			widget.Focus = false;

			if (dispatched)
				return true;
		}

		return false;
	}
}
