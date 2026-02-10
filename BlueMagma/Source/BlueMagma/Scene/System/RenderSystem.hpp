#pragma once
#include "ISystem.hpp"
#include "Scene/Component/Base.hpp"

namespace BM
{
	class RenderSystem : public ISystem
	{
	public:
		virtual void OnRender(const Scene& scene, sf::RenderTarget& target) const noexcept override;
	private:
		void DrawRect(sf::RenderTarget& target, const Component::Transform& transform,
			Component::RectRender rect, Component::Style style) const noexcept;

		void DrawCircle(sf::RenderTarget& target, const Component::Transform& transform,
			Component::CircleRender circle, Component::Style style) const noexcept;

		void DrawTexture(sf::RenderTarget& target, const Component::Transform& transform,
			const Component::TextureRender& texture, Component::Style style) const noexcept;

		void DrawText(sf::RenderTarget& target, const Component::Transform& transform,
			const Component::TextRender& text, Component::Style style) const noexcept;
	};
}
