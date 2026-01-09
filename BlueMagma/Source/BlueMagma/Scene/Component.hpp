#pragma once
#include "System/Vec2.hpp"
#include "Asset/Asset.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <string>
#include <string_view>
#include <cstdint>

namespace BM::Component
{
	struct Transform
	{
		Vec2f _Position;
		float _Z;
		Vec2f _Scale;

		inline Transform(Vec2f position = Vec2f::Zero(), float z = 0.f, Vec2f scale = Vec2f(1.f)) noexcept
			: _Position(position), _Z(z), _Scale(scale) {
		}
	};

	struct RectRender
	{
		Vec2f _Size;
		sf::Color _Color;

		inline RectRender(Vec2f size = Vec2f::Zero(), sf::Color color = sf::Color::White) noexcept
			: _Size(size), _Color(color) {
		}

		inline void Draw(sf::RenderTarget& target, const Transform& transform = {}) const noexcept {
			sf::RectangleShape shape{ _Size };
			shape.setFillColor(_Color);

			sf::RenderStates states;
			states.transform.translate(transform._Position);
			states.transform.scale(transform._Scale);

			target.draw(shape, states);
		}
	};

	struct CircleRender
	{
		float _Radius;
		sf::Color _Color;

		inline CircleRender(float radius = 0.f, sf::Color color = sf::Color::White) noexcept
			: _Radius(radius), _Color(color) {
		}

		inline void Draw(sf::RenderTarget& target, const Transform& transform = {}) const noexcept {
			sf::CircleShape shape{ _Radius };
			shape.setFillColor(_Color);

			sf::RenderStates states;
			states.transform.translate(transform._Position);
			states.transform.scale(transform._Scale);

			target.draw(shape, states);
		}
	};

	struct TextureRender
	{
		const Texture* _TexturePtr = nullptr;
		sf::Color _Color;

		inline TextureRender(const Texture& texture, sf::Color color = sf::Color::White) noexcept
			: _TexturePtr(&texture), _Color(color) {
		}

		inline void Draw(sf::RenderTarget& target, const Transform& transform = {}) const noexcept {
			if (!_TexturePtr)
				return;

			sf::Sprite sprite{ *_TexturePtr };
			sprite.setColor(_Color);

			sf::RenderStates states;
			states.transform.translate(transform._Position);
			states.transform.scale(transform._Scale);

			target.draw(sprite, states);
		}
	};

	struct TextRender
	{
		const Font* _FontPtr = nullptr;
		std::string _Text;
		uint32_t _CharSize;
		sf::Color _Color;

		inline TextRender(const Font& font, std::string_view text = "", uint32_t charSize = 30u, sf::Color color = sf::Color::White) noexcept
			: _FontPtr(&font), _Text(text), _CharSize(charSize), _Color(color) {
		}

		inline void Draw(sf::RenderTarget& target, const Transform& transform = {}) const noexcept {
			if (!_FontPtr)
				return;

			sf::Text text(*_FontPtr, _Text, _CharSize);
			text.setFillColor(_Color);

			sf::RenderStates states;
			states.transform.translate(transform._Position);
			states.transform.scale(transform._Scale);

			target.draw(text, states);
		}
	};
}
