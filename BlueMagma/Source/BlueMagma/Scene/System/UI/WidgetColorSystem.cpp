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
		auto color = registry.try_get<ColorMaterial>(entity);
		auto widget = registry.try_get<Widget>(entity);
		auto widgetColor = registry.try_get<WidgetColor>(entity);
		if (!color || !widget || !widgetColor)
			return;

		const auto [cIdleColor, cHoverColor, cFocusColor] = *widgetColor;
		color->Color =
			widget->Hover && cHoverColor != cIdleColor ? cHoverColor :
			widget->Focus ? cFocusColor :
			cIdleColor;
	}
}
