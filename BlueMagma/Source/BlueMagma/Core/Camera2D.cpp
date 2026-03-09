#include "bmpch.hpp"
#include "Camera2D.hpp"

namespace BM
{
	Camera2D::Camera2D(const sf::View& view, float zoomFactor) noexcept
		: Camera2D(view.getSize(), view.getCenter(), zoomFactor)
	{
	}

	Camera2D::Camera2D(RectFloat rect) noexcept
		: Camera2D(rect.Size, rect.Center())
	{
	}

	Camera2D::Camera2D(Vec2f size) noexcept
		: Camera2D(size, size.Center())
	{
	}

	Camera2D::Camera2D(Vec2f size, Vec2f center, float zoomFactor) noexcept
		: m_Size(size), m_Center(center), m_ZoomFactor(zoomFactor), m_Viewport(0.f, 0.f, 1.f, 1.f)
	{
	}

	bool Camera2D::OnResizeEvent(const EventHandle::Resized& resized) noexcept
	{
		SetSize(resized.size);
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

	void Camera2D::Zoom(float percentage) noexcept
	{
		m_ZoomFactor *= percentage;
	}

	void Camera2D::ZoomIn(float percentage, float max) noexcept
	{
		m_ZoomFactor = std::min(m_ZoomFactor * percentage, max);
	}

	void Camera2D::ZoomOut(float percentage, float min) noexcept
	{
		m_ZoomFactor = std::max(m_ZoomFactor * percentage, min);
	}

	void Camera2D::SetViewport(RectFloat viewport) noexcept
	{
		m_Viewport = viewport;
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

	sf::View Camera2D::GetView() const noexcept
	{
		sf::View view{ m_Center, m_Size / m_ZoomFactor };
		view.setViewport(m_Viewport);

		return view;
	}
}
