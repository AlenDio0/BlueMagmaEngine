#pragma once
#include "System/Vec2.hpp"
#include "Asset/Asset.hpp"
#include "System/Rect.hpp"
#include <SFML/Graphics/Color.hpp>
#include <string>
#include <cstdint>

namespace BM::Component
{
	struct Transform
	{
		Vec2f _LocalPosition{};
		Vec2f _Position{};
		float _Z = 0.f;
		Vec2f _Scale{ 1.f };
		// TODO: Vec3 so that _Z is integrated with LocalPosition

		inline Transform(Vec2f position = {}, float z = 0.f, Vec2f scale = { 1.f }) noexcept
			: _LocalPosition(position), _Z(z), _Scale(scale) {
		}
	};

	struct RectRender
	{
		Vec2f _Size{};
		sf::Color _Color{ sf::Color::White };
	};

	struct CircleRender
	{
		float _Radius = 0.f;
		sf::Color _Color{ sf::Color::White };
	};

	struct TextureRender
	{
		const Texture* _TexturePtr = nullptr;
		RectInt _TextureRect{};
		sf::Color _Color{ sf::Color::White };
		bool _UseRect = false;
	};

	struct TextRender
	{
		const Font* _FontPtr = nullptr;
		std::string _Content{ "" };
		uint32_t _CharSize = 30u;
		sf::Color _Color{ sf::Color::White };
	};
}
