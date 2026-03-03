#pragma once
#include "Core/Vec2.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/View.hpp>

namespace BM
{
	class Renderer
	{
	public:
		Renderer(sf::RenderWindow& handle) noexcept;

		void Clear(sf::Color color = sf::Color::Black) noexcept;
		void Display() noexcept;

		void Draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default) noexcept;
		void Draw(const sf::Vertex* vertices, size_t vertexCount, sf::PrimitiveType type,
			sf::RenderStates states = sf::RenderStates::Default) noexcept;

		void SetView(const sf::View& view) noexcept;

		Vec2f PixelToCoords(Vec2i point) const noexcept;
		Vec2i CoordsToPixel(Vec2f coords) const noexcept;

		Vec2u GetSize() const noexcept;
		const sf::View& GetView() const noexcept;
		const sf::View& GetDefaultView() const noexcept;

		sf::RenderWindow& GetHandle() noexcept;
		operator sf::RenderWindow& () noexcept;
	private:
		sf::RenderWindow* m_HandlePtr = nullptr;
	};
}
