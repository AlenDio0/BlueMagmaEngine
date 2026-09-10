#pragma once
#include "Camera2D.hpp"
#include "WindowHandle.hpp"

#include "Math/Color.hpp"
#include "Math/Vec2.hpp"

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
		explicit Renderer(std::weak_ptr<WindowHandle> handle) noexcept;

		void Clear(Color color = ColorDef::Black) noexcept;
		void Display() noexcept;

		void Draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default) noexcept;
		void Draw(const sf::Vertex* vertices, size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates& states = sf::RenderStates::Default) noexcept;

		void SetView(const sf::View& view) noexcept;
		void SetCamera(const Camera2D& camera) noexcept;

		void ResetCamera() noexcept;

		Vec2f PixelToCoords(Vec2i point) const noexcept;
		Vec2f PixelToCoords(Vec2i point, const Camera2D& camera) const noexcept;
		Vec2i CoordsToPixel(Vec2f coords) const noexcept;
		Vec2i CoordsToPixel(Vec2f coords, const Camera2D& camera) const noexcept;

		Vec2u GetSize() const noexcept;
		const Camera2D& GetCamera() const noexcept;
		Camera2D GetDefaultCamera() const noexcept;

		std::weak_ptr<WindowHandle> GetHandle() const noexcept;
	private:
		std::shared_ptr<WindowHandle> GetHandleRef() const noexcept;
	private:
		std::weak_ptr<WindowHandle> m_Handle;
		Camera2D m_Camera;
	};
}
