#include "bmpch.hpp"
#include "RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace BM
{
	void RenderSystem::OnRender(const Scene& scene, sf::RenderTarget& target) const noexcept
	{
		using namespace Component;

		m_CachedRectBatch.reserve(scene.View<RectRender>().size() * 6);

		auto view = scene.View<Transform>();
		for (auto entity : view)
		{
			const Transform& transform = view.get<Transform>(entity);

			if (auto rect = scene.TryGetComponent<RectRender>(entity))
			{
				AddRect(rect, transform);
			}
			else if (auto circle = scene.TryGetComponent<CircleRender>(entity))
			{
				Flush(target);
				DrawCircle(target, *circle, transform);
			}
			else if (auto texture = scene.TryGetComponent<TextureRender>(entity))
			{
				Flush(target);
				DrawTexture(target, *texture, transform);
			}
			else if (auto text = scene.TryGetComponent<TextRender>(entity))
			{
				Flush(target);
				DrawText(target, *text, transform);
			}
		}

		Flush(target);
	}

	void RenderSystem::AddRect(const Component::RectRender* rect, const Component::Transform& transform) const noexcept
	{
		const Vec2f cPosition = transform._Position;
		const Vec2f cSize = cPosition + (rect->_Size * transform._Scale);
		const sf::Color cColor = rect->_Color;

		Vec2f topLeft(cPosition);
		Vec2f topRight(cSize._X, cPosition._Y);
		Vec2f bottomRight(cSize);
		Vec2f bottomLeft(cPosition._X, cSize._Y);

		m_CachedRectBatch.emplace_back(topLeft, cColor);
		m_CachedRectBatch.emplace_back(topRight, cColor);
		m_CachedRectBatch.emplace_back(bottomRight, cColor);
		m_CachedRectBatch.emplace_back(topLeft, cColor);
		m_CachedRectBatch.emplace_back(bottomRight, cColor);
		m_CachedRectBatch.emplace_back(bottomLeft, cColor);
	}

	void RenderSystem::Flush(sf::RenderTarget& target) const noexcept
	{
		target.draw(m_CachedRectBatch.data(), m_CachedRectBatch.size(), sf::PrimitiveType::Triangles);
		m_CachedRectBatch.clear();
	}

	static inline sf::RenderStates GetRenderState(const Component::Transform& transform)
	{
		sf::RenderStates states;
		return states.transform.translate(transform._Position).scale(transform._Scale);
	}

	static inline sf::CircleShape s_Circle;
	void RenderSystem::DrawCircle(sf::RenderTarget& target, const Component::CircleRender& circle, const Component::Transform& transform) const noexcept
	{
		s_Circle.setRadius(circle._Radius);
		s_Circle.setFillColor(circle._Color);

		target.draw(s_Circle, GetRenderState(transform));
	}

	static inline sf::Sprite s_Sprite{ Texture::GetDefault() };
	void RenderSystem::DrawTexture(sf::RenderTarget& target, const Component::TextureRender& texture, const Component::Transform& transform) const noexcept
	{
		if (!texture._TexturePtr)
			return;

		s_Sprite.setTexture(*texture._TexturePtr, true);
		if (texture._UseRect)
			s_Sprite.setTextureRect(texture._TextureRect);

		s_Sprite.setColor(texture._Color);

		target.draw(s_Sprite, GetRenderState(transform));
	}

	static inline sf::Text s_Text{ Font::GetDefault() };
	void RenderSystem::DrawText(sf::RenderTarget& target, const Component::TextRender& text, const Component::Transform& transform) const noexcept
	{
		if (!text._FontPtr)
			return;

		s_Text.setFont(*text._FontPtr);
		s_Text.setString(text._Content);
		s_Text.setCharacterSize(text._CharSize);
		s_Text.setFillColor(text._Color);

		target.draw(s_Text, GetRenderState(transform));
	}
}