#pragma once
#include "Vec2.hpp"
#include "Base/EventDispatcher.hpp"
#include <SFML/Graphics/View.hpp>

namespace BM
{
	class Camera2D
	{
	public:
		Camera2D(Vec2f size) noexcept;
		Camera2D(const sf::View& view, float zoomFactor) noexcept;
		Camera2D(Vec2f size, Vec2f center, float zoomFactor) noexcept;

		bool OnResizeEvent(const EventHandle::Resized& resized) noexcept;

		void SetSize(Vec2f size) noexcept;

		void SetCenter(Vec2f center) noexcept;
		void Move(Vec2f offset) noexcept;

		void SetZoomFactor(float zoomFactor) noexcept;
		void Zoom(float factor) noexcept;
		void ZoomIn(float amount, float min);
		void ZoomOut(float amount, float max);

		Vec2f GetSize() const noexcept;
		Vec2f GetCenter() const noexcept;
		float GetZoomFactor() const noexcept;

		sf::View GetView() const noexcept;
	private:
		Vec2f m_Size;

		Vec2f m_Center;
		float m_ZoomFactor;
	};
}
