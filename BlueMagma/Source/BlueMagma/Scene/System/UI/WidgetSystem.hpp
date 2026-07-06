#pragma once
#include "Scene/System/ISystem.hpp"
#include "Base/EventDispatcher.hpp"
#include "Scene/EntityHandle.hpp"
#include "Scene/Component/Base.hpp"
#include "Scene/Component/UI.hpp"
#include "Core/Vec2.hpp"
#include <functional>

namespace BM::UI
{
	class WidgetSystem : public ISystem
	{
	public:
		virtual void OnAttach(Scene& scene) noexcept override;

		virtual void OnEvent(Scene& scene, Event& event) noexcept override;
		virtual void OnUpdate(Scene& scene, float deltaTime) noexcept override;

		static bool Contains(Registry& registry, const Component::Transform& transform, const Component::Widget& widget, Vec2i point) noexcept;
	private:
		bool OnMouseMoved(const EventHandle::MouseMoved& mouseMoved) noexcept;
		bool OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept;

		using WidgetUpdateFn = std::function<void(EntityHandle, const Component::Widget&, bool)>;
		void UpdateWidgets(Registry& registry, const WidgetUpdateFn& onUpdate) const noexcept;

		void UpdateTransformWidget() noexcept;
	private:
		bool m_UpdatedHover = false;

		Vec2i m_MousePosition{};
	};
}
