#include "bmpch.hpp"
#include "WidgetColorSystem.hpp"
#include "Scene/Component/Base.hpp"
#include "Scene/Component/UI.hpp"

namespace BM::UI
{
	using namespace Component;

	void WidgetColorSystem::OnAttach(Scene& scene) noexcept
	{
		scene.OnConstruct<Widget>().connect<&WidgetColorSystem::TryUpdateColor>(this);
		scene.OnUpdate<Widget>().connect<&WidgetColorSystem::TryUpdateColor>(this);

		scene.OnConstruct<WidgetColor>().connect<&WidgetColorSystem::TryUpdateColor>(this);
		scene.OnUpdate<WidgetColor>().connect<&WidgetColorSystem::TryUpdateColor>(this);
	}

	void WidgetColorSystem::TryUpdateColor(Registry& registry, EntityHandle entity) noexcept
	{
		auto style = registry.try_get<Style>(entity);
		auto widget = registry.try_get<Widget>(entity);
		auto widgetColor = registry.try_get<WidgetColor>(entity);
		if (!style || !widget || !widgetColor)
			return;

		style->FillColor =
			widget->Hover ? widgetColor->HoverColor :
			widget->Focus ? widgetColor->FocusColor :
			widgetColor->IdleColor;
	}
}
