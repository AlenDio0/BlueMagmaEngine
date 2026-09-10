#include "bmpch.hpp"
#include "Renderer.hpp"

namespace BM
{
	Renderer::Renderer(std::weak_ptr<WindowHandle> handle) noexcept
		: m_Handle(handle), m_Camera(GetDefaultCamera())
	{
	}

	void Renderer::Clear(Color color) noexcept
	{
		GetHandleRef()->clear(color);
	}

	void Renderer::Display() noexcept
	{
		GetHandleRef()->display();
	}

	void Renderer::Draw(const sf::Drawable& drawable, const sf::RenderStates& states) noexcept
	{
		GetHandleRef()->draw(drawable, states);
	}

	void Renderer::Draw(const sf::Vertex* vertices, size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states) noexcept
	{
		GetHandleRef()->draw(vertices, vertexCount, type, states);
	}

	void Renderer::SetView(const sf::View& view) noexcept
	{
		GetHandleRef()->setView(view);
	}

	void Renderer::SetCamera(const Camera2D& camera) noexcept
	{
		m_Camera = camera;
		m_Camera.SetCenter(m_Camera.GetCenter().Round());
		SetView(m_Camera.GetView());
	}

	void Renderer::ResetCamera() noexcept
	{
		SetCamera(GetDefaultCamera());
	}

	Vec2f Renderer::PixelToCoords(Vec2i point) const noexcept
	{
		return GetHandleRef()->mapPixelToCoords(point);
	}

	Vec2f Renderer::PixelToCoords(Vec2i point, const Camera2D& camera) const noexcept
	{
		return GetHandleRef()->mapPixelToCoords(point, camera.GetView());
	}

	Vec2i Renderer::CoordsToPixel(Vec2f coords) const noexcept
	{
		return GetHandleRef()->mapCoordsToPixel(coords);
	}

	Vec2i Renderer::CoordsToPixel(Vec2f coords, const Camera2D& camera) const noexcept
	{
		return GetHandleRef()->mapCoordsToPixel(coords, camera.GetView());
	}

	Vec2u Renderer::GetSize() const noexcept
	{
		return GetHandleRef()->getSize();
	}

	const Camera2D& Renderer::GetCamera() const noexcept
	{
		return m_Camera;
	}

	Camera2D Renderer::GetDefaultCamera() const noexcept
	{
		return Camera2D(GetHandleRef()->getDefaultView());
	}

	std::weak_ptr<sf::RenderWindow> Renderer::GetHandle() const noexcept
	{
		return GetHandleRef();
	}

	std::shared_ptr<WindowHandle> Renderer::GetHandleRef() const noexcept
	{
		BM_CORE_ASSERT(!m_Handle.expired(), "Window Handle not created");
		return m_Handle.lock();
	}
}
