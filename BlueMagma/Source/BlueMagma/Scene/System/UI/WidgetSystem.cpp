#include "bmpch.hpp"
#include "WidgetSystem.hpp"
#include "Scene/Scene.hpp"

namespace BM::UI
{
	using namespace Component;

	void WidgetSystem::OnAttach(Scene& scene) noexcept
	{
		scene.OnDestroy<Transform>().connect<&WidgetSystem::UpdateTransformWidget>(this);

		scene.OnUpdate<Transform>().connect<&WidgetSystem::UpdateTransformWidget>(this);
	}

	void WidgetSystem::OnEvent(Scene& scene, Event& event) noexcept
	{
		BM::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<BM::EventHandle::MouseMoved>(BM_EVENT_FN(OnMouseMoved, scene));
		dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed, scene));
	}

	void WidgetSystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		if (m_UpdatedHover)
			return;

		UpdateWidgets(scene.GetRegistry(), [&](auto entity, const auto& widget, bool onMouse) {
			if (widget.Hover != onMouse)
				scene.PatchComponent<Widget>(entity, [&](auto& widget) { widget.Hover = onMouse; });
			});

		m_UpdatedHover = true;
	}

	bool WidgetSystem::Contains(Registry& registry, const Transform& transform, const Widget& widget, Vec2i point) noexcept
	{
		const auto& origin = transform.Local.State.Origin;
		const auto& [position, scale, rotation, z] = transform.Global;
		const Vec2f cSize = widget.Size;

		Vec2f coords = point;
		if (auto renderer = registry.ctx().get<Renderer*>())
			coords = renderer->PixelToCoords(point);

		const auto cMatrix = Transform2D::ToMatrix({ position, scale, origin, rotation }, { {0.f}, cSize });
		const Vec2f cPosition = cMatrix.getInverse().transformPoint(coords);

		switch (widget.Shape)
		{
			using Shape = Widget::ShapeType;

		case Shape::Rect:
			return (cPosition.X >= 0.f && cPosition.X <= cSize.X) && (cPosition.Y >= 0.f && cPosition.Y <= cSize.Y);
		case Shape::Circle:
		{
			const Vec2f cSemiAxes = cSize / 2.f;

			return ((cPosition - cSemiAxes).Squared() / cSemiAxes.Squared()).Sum() <= 1.f;
		}

		default:
			return false;
		}
	}

	bool WidgetSystem::OnMouseMoved(const EventHandle::MouseMoved& mouseMoved, Scene& scene) noexcept
	{
		m_MousePosition = mouseMoved.position;
		m_UpdatedHover = false;

		return false;
	}

	bool WidgetSystem::OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept
	{
		m_MousePosition = mousePressed.position;

		UpdateWidgets(scene.GetRegistry(), [&](auto entity, const auto& widget, bool onMouse) {
			if (widget.Focus != onMouse)
				scene.PatchComponent<Widget>(entity, [&](auto& widget) { widget.Focus = onMouse; });
			});

		return false;
	}

	void WidgetSystem::UpdateWidgets(Registry& registry, const WidgetUpdateFn& onUpdate) const noexcept
	{
		if (!onUpdate)
			return;

		bool handled = false;

		auto view = registry.view<Transform, Widget>();
		view.use<Transform>();

		auto viewVector = Scene::ViewToVector(view);
		for (auto entity : viewVector | std::views::reverse)
		{
			const auto& transform = view.get<Transform>(entity);
			const auto& widget = view.get<Widget>(entity);

			const bool cOnMouse = !handled && Contains(registry, transform, widget, m_MousePosition);
			onUpdate(entity, widget, cOnMouse);

			handled = handled || cOnMouse;
		}
	}

	void WidgetSystem::UpdateTransformWidget(Registry& registry, EntityHandle entity) noexcept
	{
		m_UpdatedHover = false;
	}
}
