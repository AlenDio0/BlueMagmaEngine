#pragma once
#include "Core/Vec2.hpp"
#include "Core/Transform2D.hpp"
#include "Asset/Asset.hpp"
#include "Core/Rect.hpp"
#include <SFML/Graphics/Color.hpp>
#include <string>
#include <cstdint>
#include <optional>

namespace BM::Component
{
	struct Transform
	{
		struct LocalSpace
		{
			Transform2D State{};
			float Z = 0.f;
		} Local;

		struct GlobalSpace
		{
			Vec2f Position{ 0.f };
			Vec2f Scale{ 1.f };
			float Rotation = 0.f;
			float Z = 0.f;
		} Global;

		bool CachedUpdated = false;
	};

	struct Hidden
	{
		bool Visible = false;
	};

	struct ColorMaterial
	{
		sf::Color Color = sf::Color::White;
	};

	struct TextureMaterial
	{
		const Texture* TexturePtr = nullptr;
		std::optional<RectInt> TextureRect{};
	};

	struct Outline
	{
		sf::Color Color = sf::Color::Transparent;
		float Thickness = 0.f;
	};

	struct RectShape
	{
		Vec2f Size{};
		float Corner = 0.f;
	};

	struct CircleShape
	{
		float Radius = 0.f;
	};

	struct SpriteShape
	{
		const Texture* TexturePtr = nullptr;
		std::optional<RectInt> TextureRect{};
	};

	struct TextRender
	{
		const Font* FontPtr = nullptr;
		std::string Text{ "" };
		uint32_t CharSize = 30u;
	};
}
