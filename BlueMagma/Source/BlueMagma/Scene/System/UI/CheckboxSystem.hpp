#pragma once
#include "Scene/System/ISystem.hpp"

#include "Scene/Component/UI.hpp"

#include <optional>

namespace BM::UI
{
	class CheckboxSystem : public ISystem
	{
	public:
		virtual void OnEvent(Scene& scene, Event& event) noexcept override;

		static std::optional<Component::ColorMaterial> TryGetColorMaterial(const Component::Checkbox::MaterialVariant& material) noexcept;
		static std::optional<Component::TextureMaterial> TryGetTextureMaterial(const Component::Checkbox::MaterialVariant& material) noexcept;
	private:
		bool OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept;
	};
}
