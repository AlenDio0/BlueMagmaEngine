#pragma once
#include "ISystem.hpp"
#include "Scene/Component.hpp"
#include <SFML/Graphics/Vertex.hpp>
#include <vector>

namespace BM
{
	class RenderSystem : public ISystem
	{
	public:
		virtual void OnRender(const Scene& scene, sf::RenderTarget& target) const noexcept override;

		static void DrawCircle(sf::RenderTarget& target, const Component::CircleRender& circle, const Component::Transform& transform) noexcept;
		static void DrawTexture(sf::RenderTarget& target, const Component::TextureRender& texture, const Component::Transform& transform) noexcept;
		static void DrawText(sf::RenderTarget& target, const Component::TextRender& text, const Component::Transform& transform) noexcept;
	private:
		static void AddRect(std::vector<sf::Vertex>& batch, const Component::RectRender* rect, const Component::Transform& transform) noexcept;
		static void Flush(sf::RenderTarget& target, std::vector<sf::Vertex>& batch) noexcept;
	};
}
