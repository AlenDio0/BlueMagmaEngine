#include "bmpch.hpp"
#include "ClickableSystem.hpp"
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
			if (!widget.Focus || !onClick)
				continue;

			const bool cDispatched = onClick(scene.GetEntity(entity), mousePressed);
			widget.Focus = false;

			if (cDispatched)
				return true;
		}

		return false;
	}
}
