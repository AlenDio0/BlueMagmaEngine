#include "bmpch.hpp"
#include "Renderer.hpp"
#include "Core/Assert.hpp"

namespace BM
{
	Renderer::Renderer(sf::RenderWindow& handle) noexcept
		: m_HandlePtr(&handle)
	{
	}

	void Renderer::Clear(sf::Color color) noexcept
	{
		m_HandlePtr->clear(color);
	}

	void Renderer::Display() noexcept
	{
		m_HandlePtr->display();
	}

	void Renderer::Draw(const sf::Drawable& drawable, const sf::RenderStates& states) noexcept
	{
		m_HandlePtr->draw(drawable, states);
	}

	void Renderer::Draw(const sf::Vertex* vertices, size_t vertexCount, sf::PrimitiveType type, sf::RenderStates states) noexcept
	{
		m_HandlePtr->draw(vertices, vertexCount, type, states);
	}

	void Renderer::SetView(const sf::View& view) noexcept
	{
		m_HandlePtr->setView(view);
	}

	Vec2f Renderer::PixelToCoords(Vec2i point) const noexcept
	{
		return m_HandlePtr->mapPixelToCoords(point);
	}

	Vec2i Renderer::CoordsToPixel(Vec2f coords) const noexcept
	{
		return m_HandlePtr->mapCoordsToPixel(coords);
	}

	Vec2u Renderer::GetSize() const noexcept
	{
		return m_HandlePtr->getSize();
	}

	const sf::View& Renderer::GetView() const noexcept
	{
		return m_HandlePtr->getView();
	}

	const sf::View& Renderer::GetDefaultView() const noexcept
	{
		return m_HandlePtr->getDefaultView();
	}

	sf::RenderWindow& Renderer::GetHandle() noexcept
	{
		BM_CORE_ASSERT(m_HandlePtr != nullptr, "Window Handle not contructed yet");
		return *m_HandlePtr;
	}

	Renderer::operator sf::RenderWindow& () noexcept
	{
		return GetHandle();
	}
}
