#include "bmpch.hpp"
#include "WidgetSystem.hpp"
#include "Scene/Scene.hpp"

namespace BM::UI
{
	using namespace Component;

	void WidgetSystem::OnAttach(Scene& scene) noexcept
	{
		scene.OnConstruct<Transform>().connect<&WidgetSystem::SetNeedUpdateHover>(this);
		scene.OnUpdate<Transform>().connect<&WidgetSystem::SetNeedUpdateHover>(this);
		scene.OnDestroy<Transform>().connect<&WidgetSystem::SetNeedUpdateHover>(this);

		scene.OnConstruct<Widget>().connect<&WidgetSystem::SetNeedUpdateHover>(this);
		scene.OnUpdate<Widget>().connect<&WidgetSystem::SetNeedUpdateHover>(this);
		scene.OnDestroy<Widget>().connect<&WidgetSystem::SetNeedUpdateHover>(this);
	}

	void WidgetSystem::OnEvent(Scene& scene, Event& event) noexcept
	{
		BM::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<BM::EventHandle::MouseMoved>(BM_EVENT_FN(OnMouseMoved));
		dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed, scene));
	}

	void WidgetSystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		if (!m_NeedUpdateHover)
			return;

		UpdateWidgets(scene.GetRegistry(), [&](auto entity, const auto& widget, bool onMouse) {
			if (widget.Hover != onMouse)
				scene.PatchComponent<Widget>(entity, [&](auto& widget) { widget.Hover = onMouse; });
			});

		m_NeedUpdateHover = false;
	}

	bool WidgetSystem::Contains(Registry& registry, const Transform& transform, const Widget& widget, Vec2i point) noexcept
	{
		const auto& origin = transform.Local.State.Origin;
		const auto& [position, scale, rotation, z] = transform.Global;
		const Vec2f cSize = widget.Size;

		Vec2f coords = point;
		if (auto renderer = registry.ctx().get<Renderer*>())
			coords = renderer->PixelToCoords(point);

		const auto cMatrix = Transform2D::ToMatrix({ position, scale, origin, rotation }, { Vec2f(0.f), cSize });
		const Vec2f cCoordsPosition = cMatrix.getInverse().transformPoint(coords);

		switch (widget.Shape)
		{
			using Shape = Widget::ShapeType;

		case Shape::Rect:
			return RectFloat(Vec2f::Zero(), cSize).Contains(cCoordsPosition);
		case Shape::Circle:
		{
			const Vec2f cSemiAxes = cSize / 2.f;
			return ((cCoordsPosition - cSemiAxes).Squared() / cSemiAxes.Squared()).Sum() <= 1.f;
		}

		default:
			BM_CORE_NOT_IMPLEMENTED();
			return false;
		}
	}

	bool WidgetSystem::OnMouseMoved(const EventHandle::MouseMoved& mouseMoved) noexcept
	{
		m_MousePosition = mouseMoved.position;
		m_NeedUpdateHover = true;

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

	void WidgetSystem::SetNeedUpdateHover() noexcept
	{
		m_NeedUpdateHover = true;
	}
}
