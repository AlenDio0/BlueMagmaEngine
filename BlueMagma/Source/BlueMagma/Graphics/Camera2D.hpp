#pragma once
#include "Math/Vec2.hpp"
#include "Math/Rect.hpp"
#include "Event/Event.hpp"

#include <SFML/Graphics/View.hpp>

namespace BM
{
	class Camera2D
	{
	public:
		Camera2D() noexcept;
		explicit Camera2D(RectFloat rect) noexcept;
		explicit Camera2D(Vec2f size) noexcept;
		explicit Camera2D(const sf::View& view, float zoomFactor = 1.f) noexcept;
		Camera2D(Vec2f size, Vec2f center, float zoomFactor = 1.f) noexcept;

		bool OnViewportResizeEvent(const EventHandle::Resized& resized) noexcept;

		void SetSize(Vec2f size) noexcept;

		void SetCenter(Vec2f center) noexcept;
		void Move(Vec2f offset) noexcept;

		void SetZoomFactor(float zoomFactor) noexcept;
		void Zoom(float multiplier) noexcept;
		void ZoomIn(float amount, float min) noexcept;
		void ZoomOut(float amount, float max) noexcept;

		void SetViewport(RectFloat viewport) noexcept;
		void SetViewport(RectFloat viewport, Vec2u windowSize) noexcept;

		[[nodiscard]] Vec2f GetSize() const noexcept;
		[[nodiscard]] Vec2f GetCenter() const noexcept;
		[[nodiscard]] float GetZoomFactor() const noexcept;
		[[nodiscard]] RectFloat GetViewport() const noexcept;

		[[nodiscard]] bool Contains(Vec2f coords) const noexcept;
		[[nodiscard]] bool Contains(Vec2i point, Vec2u windowSize) const noexcept;

		[[nodiscard]] RectFloat GetBounds() const noexcept;
		[[nodiscard]] sf::View GetView() const noexcept;
	private:
		Vec2f m_Size;
		Vec2f m_Center;
		float m_ZoomFactor;

		RectFloat m_Viewport;
	};
}
