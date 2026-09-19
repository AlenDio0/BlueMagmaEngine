#include "bmpch.hpp"
#include "CheckboxSystem.hpp"

#include "Event/EventDispatcher.hpp"

namespace BM::UI
{
	using namespace Component;

	void CheckboxSystem::OnEvent(Scene& scene, Event& event) noexcept
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed, scene));
	}

	std::optional<ColorMaterial> CheckboxSystem::TryGetColorMaterial(const Checkbox::MaterialVariant& material) noexcept
	{
		if (!std::holds_alternative<ColorMaterial>(material))
			return std::nullopt;

		return std::get<ColorMaterial>(material);
	}

	std::optional<TextureMaterial> CheckboxSystem::TryGetTextureMaterial(const Checkbox::MaterialVariant& material) noexcept
	{
		if (!std::holds_alternative<TextureMaterial>(material))
			return std::nullopt;

		return std::get<TextureMaterial>(material);
	}

	bool CheckboxSystem::OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept
	{
		auto view = scene.View<Widget, Checkbox>();
		for (auto [entity, widget, checkbox] : view.each())
		{
			const auto& onChanged = checkbox.OnChanged;
			if (!widget.Focus)
				continue;

			bool dispatched = false;

			bool& active = checkbox.Active;

			active = !active;
			if (onChanged)
				dispatched = onChanged(scene.GetEntity(entity), scene.GetEntity(checkbox.CheckChild), mousePressed, active);

			widget.Focus = false;

			if (Entity check = checkbox.CheckChild)
			{
				check.TryPatch<ColorMaterial>([&](auto& material) {
					if (active)
						material = TryGetColorMaterial(checkbox.ActiveMaterial).value_or({});
					else if (!active)
						material = TryGetColorMaterial(checkbox.InactiveMaterial).value_or({});
					});

				check.TryPatch<TextureMaterial>([&](auto& material) {
					if (active)
						material = TryGetTextureMaterial(checkbox.ActiveMaterial).value_or({});
					else if (!active)
						material = TryGetTextureMaterial(checkbox.InactiveMaterial).value_or({});
					});
			}

			if (dispatched)
				return true;
		}

		return false;
	}
}