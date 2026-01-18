#include "bmpch.hpp"
#include "RenderSystem.hpp"
#include "../Scene.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>

namespace BM
{
	void RenderSystem::OnRender(const Scene& scene, sf::RenderTarget& target) const noexcept
	{
		using namespace Component;

		auto view = scene.View<Transform>();

		std::vector<sf::Vertex> rectBatch;
		rectBatch.reserve(scene.View<RectRender>().size() * 6);

		for (auto entity : view)
		{
			const Transform& transform = view.get<Transform>(entity);

			if (auto rect = scene.TryGetComponent<RectRender>(entity))
			{
				AddRect(rectBatch, rect, transform);
			}
			else if (auto circle = scene.TryGetComponent<CircleRender>(entity))
			{
				Flush(target, rectBatch);
				DrawCircle(target, *circle, transform);
			}
			else if (auto texture = scene.TryGetComponent<TextureRender>(entity))
			{
				Flush(target, rectBatch);
				DrawTexture(target, *texture, transform);
			}
			else if (auto text = scene.TryGetComponent<TextRender>(entity))
			{
				Flush(target, rectBatch);
				DrawText(target, *text, transform);
			}
		}

		Flush(target, rectBatch);
	}

	static inline sf::RenderStates GetRenderState(const Component::Transform& transform)
	{
		sf::RenderStates states;
		return states.transform.translate(transform._Position).scale(transform._Scale);
	}

	static inline sf::CircleShape s_Circle;
	void RenderSystem::DrawCircle(sf::RenderTarget& target, const Component::CircleRender& circle, const Component::Transform& transform) noexcept
	{
		s_Circle.setRadius(circle._Radius);
		s_Circle.setFillColor(circle._Color);

		target.draw(s_Circle, GetRenderState(transform));
	}

	static inline sf::Sprite s_Sprite{ (Texture&)Texture() };
	void RenderSystem::DrawTexture(sf::RenderTarget& target, const Component::TextureRender& texture, const Component::Transform& transform) noexcept
	{
		if (!texture._TexturePtr)
			return;

		s_Sprite.setTexture(*texture._TexturePtr, true);
		if (texture._UseRect)
			s_Sprite.setTextureRect(texture._TextureRect);

		s_Sprite.setColor(texture._Color);

		target.draw(s_Sprite, GetRenderState(transform));
	}

	static inline sf::Text s_Text{ (Font&)Font() };
	void RenderSystem::DrawText(sf::RenderTarget& target, const Component::TextRender& text, const Component::Transform& transform) noexcept
	{
		if (!text._FontPtr)
			return;

		s_Text.setFont(*text._FontPtr);
		s_Text.setString(text._Content);
		s_Text.setCharacterSize(text._CharSize);
		s_Text.setFillColor(text._Color);

		target.draw(s_Text, GetRenderState(transform));
	}

	void RenderSystem::AddRect(std::vector<sf::Vertex>& batch, const Component::RectRender* rect, const Component::Transform& transform) noexcept
	{
		const Vec2f cPosition = transform._Position;
		const Vec2f cSize = cPosition + (rect->_Size * transform._Scale);
		const sf::Color cColor = rect->_Color;

		Vec2f topLeft(cPosition);
		Vec2f topRight(cSize._X, cPosition._Y);
		Vec2f bottomRight(cSize);
		Vec2f bottomLeft(cPosition._X, cSize._Y);

		batch.emplace_back(topLeft, cColor);
		batch.emplace_back(topRight, cColor);
		batch.emplace_back(bottomRight, cColor);
		batch.emplace_back(topLeft, cColor);
		batch.emplace_back(bottomRight, cColor);
		batch.emplace_back(bottomLeft, cColor);
	}
	void RenderSystem::Flush(sf::RenderTarget& target, std::vector<sf::Vertex>& batch) noexcept
	{
		if (batch.empty())
			return;

		target.draw(batch.data(), batch.size(), sf::PrimitiveType::Triangles);
		batch.clear();
	}
}