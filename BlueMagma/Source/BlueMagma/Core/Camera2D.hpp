#pragma once
#include "Vec2.hpp"
#include "Rect.hpp"
#include "Base/EventDispatcher.hpp"
#include <SFML/Graphics/View.hpp>

namespace BM
{
	class Camera2D
	{
	public:
		explicit Camera2D(RectFloat rect) noexcept;
		explicit Camera2D(Vec2f size) noexcept;
		Camera2D(const sf::View& view, float zoomFactor = 1.f) noexcept;
		Camera2D(Vec2f size, Vec2f center, float zoomFactor = 1.f) noexcept;

		bool OnResizeEvent(const EventHandle::Resized& resized) noexcept;

		void SetSize(Vec2f size) noexcept;
		void SetSizeWindow(Vec2u windowSize) noexcept;

		void SetCenter(Vec2f center) noexcept;
		void Move(Vec2f offset) noexcept;

		void SetZoomFactor(float zoomFactor) noexcept;
		void Zoom(float multiplier) noexcept;
		void ZoomIn(float amount, float min) noexcept;
		void ZoomOut(float amount, float max) noexcept;

		void SetViewport(RectFloat viewport) noexcept;

		Vec2f GetSize() const noexcept;
		Vec2f GetCenter() const noexcept;
		float GetZoomFactor() const noexcept;
		RectFloat GetViewport() const noexcept;

		bool Contains(Vec2f coords) const noexcept;
		bool Contains(Vec2i point, Vec2u windowSize) const noexcept;

		RectFloat GetBounds() const noexcept;
		sf::View GetView() const noexcept;
	private:
		Vec2f m_Size;
		Vec2f m_Center;
		float m_ZoomFactor;

		RectFloat m_Viewport;
	};
}
