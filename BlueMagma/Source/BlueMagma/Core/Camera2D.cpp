#include "bmpch.hpp"
#include "Camera2D.hpp"

namespace BM
{
	Camera2D::Camera2D(Vec2f size) noexcept
		: Camera2D(size, size.Center(), 1.f)
	{
	}

	Camera2D::Camera2D(const sf::View& view, float zoomFactor) noexcept
		: Camera2D(view.getSize(), view.getCenter(), zoomFactor)
	{
	}

	Camera2D::Camera2D(Vec2f size, Vec2f center, float zoomFactor) noexcept
		: m_Size(size), m_Center(center), m_ZoomFactor(zoomFactor)
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

	void Camera2D::Zoom(float factor) noexcept
	{
		m_ZoomFactor *= factor;
	}

	void Camera2D::ZoomIn(float amount, float min)
	{
		m_ZoomFactor = std::max(m_ZoomFactor - amount, min);
	}

	void Camera2D::ZoomOut(float amount, float max)
	{
		m_ZoomFactor = std::min(m_ZoomFactor + amount, max);
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

	sf::View Camera2D::GetView() const noexcept
	{
		return sf::View(m_Center, m_Size * m_ZoomFactor);
	}
}
