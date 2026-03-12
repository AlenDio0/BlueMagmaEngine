#include "bmpch.hpp"
#include "Camera2D.hpp"

namespace BM
{
	Camera2D::Camera2D(RectFloat rect) noexcept
		: Camera2D(rect.Size, rect.Center())
	{}

	Camera2D::Camera2D(Vec2f size) noexcept
		: Camera2D(size, size.Center())
	{}

	Camera2D::Camera2D(const sf::View& view, float zoomFactor) noexcept
		: Camera2D(view.getSize(), view.getCenter(), zoomFactor)
	{}

	Camera2D::Camera2D(Vec2f size, Vec2f center, float zoomFactor) noexcept
		: m_Size(size), m_Center(center), m_ZoomFactor(zoomFactor), m_Viewport(0.f, 0.f, 1.f, 1.f)
	{}

	bool Camera2D::OnResizeEvent(const EventHandle::Resized& resized) noexcept
	{
		SetViewport(m_Viewport, resized.size);
		return false;
	}

	void Camera2D::SetSize(Vec2f size) noexcept
	{
		m_Size = size;
	}

	void Camera2D::SetCenter(Vec2f center) noexcept
	{
		m_Center = center;
	}

	void Camera2D::Move(Vec2f offset) noexcept
	{
		m_Center += offset;
	}

	void Camera2D::SetZoomFactor(float zoomFactor) noexcept
	{
		m_ZoomFactor = zoomFactor;
	}

	void Camera2D::Zoom(float multiplier) noexcept
	{
		m_ZoomFactor *= multiplier;
	}

	void Camera2D::ZoomIn(float amount, float max) noexcept
	{
		m_ZoomFactor = std::min(m_ZoomFactor + amount, max);
	}

	void Camera2D::ZoomOut(float amount, float min) noexcept
	{
		m_ZoomFactor = std::max(m_ZoomFactor - amount, min);
	}

	void Camera2D::SetViewport(RectFloat viewport) noexcept
	{
		m_Viewport = viewport;
	}

	void Camera2D::SetViewport(RectFloat viewport, Vec2u windowSize) noexcept
	{
		SetViewport(viewport);
		SetSize(m_Viewport.Size * windowSize);
	}

	Vec2f Camera2D::GetSize() const noexcept
	{
		return m_Size;
	}

	Vec2f Camera2D::GetCenter() const noexcept
	{
		return m_Center;
	}

	float Camera2D::GetZoomFactor() const noexcept
	{
		return m_ZoomFactor;
	}

	RectFloat Camera2D::GetViewport() const noexcept
	{
		return m_Viewport;
	}

	bool Camera2D::Contains(Vec2f coords) const noexcept
	{
		return GetBounds().Contains(coords);
	}

	bool Camera2D::Contains(Vec2i point, Vec2u windowSize) const noexcept
	{
		return (m_Viewport * windowSize).Contains(Vec2f(point));
	}

	RectFloat Camera2D::GetBounds() const noexcept
	{
		const Vec2f cZoomedSize = m_Size / m_ZoomFactor;
		return RectFloat(m_Center - cZoomedSize.Center(), cZoomedSize);
	}

	sf::View Camera2D::GetView() const noexcept
	{
		sf::View view{ m_Center, m_Size / m_ZoomFactor };
		view.setViewport(m_Viewport);

		return view;
	}
}
