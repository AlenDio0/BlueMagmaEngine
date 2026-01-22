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
	private:
		void AddRect(const Component::RectRender* rect, const Component::Transform& transform) const noexcept;
		void Flush(sf::RenderTarget& target) const noexcept;

		void DrawCircle(sf::RenderTarget& target, const Component::CircleRender& circle, const Component::Transform& transform) const noexcept;
		void DrawTexture(sf::RenderTarget& target, const Component::TextureRender& texture, const Component::Transform& transform) const noexcept;
		void DrawText(sf::RenderTarget& target, const Component::TextRender& text, const Component::Transform& transform) const noexcept;
	private:
		mutable std::vector<sf::Vertex> m_CachedRectBatch;
	};
}
