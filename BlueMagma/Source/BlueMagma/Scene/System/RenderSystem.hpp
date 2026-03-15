#pragma once
#include "ISystem.hpp"
#include "Base/Renderer.hpp"
#include "Scene/Component/Base.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace BM
{
	class RenderSystem : public ISystem
	{
	public:
		virtual void OnRender(Scene& scene) const noexcept override;
	private:
		void DrawRect(Renderer& renderer, const Component::Transform& transform,
			Component::RectRender rect, Component::Style style) const noexcept;

		void DrawCircle(Renderer& renderer, const Component::Transform& transform,
			Component::CircleRender circle, Component::Style style) const noexcept;

		void DrawTexture(Renderer& renderer, const Component::Transform& transform,
			const sf::Sprite& sprite) const noexcept;

		void DrawTexture(Renderer& renderer, const Component::Transform& transform,
			const Component::TextureRender& texture, Component::Style style) const noexcept;

		void DrawText(Renderer& renderer, const Component::Transform& transform,
			const sf::Text& text) const noexcept;

		void DrawText(Renderer& renderer, const Component::Transform& transform,
			const Component::TextRender& textRender, Component::Style style) const noexcept;
	};
}
