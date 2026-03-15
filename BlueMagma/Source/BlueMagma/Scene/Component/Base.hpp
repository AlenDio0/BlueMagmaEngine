#pragma once
#include "Core/Vec2.hpp"
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
		Vec2f LocalPosition;
		float LocalZ;
		Vec2f LocalScale;
		Vec2f Origin;

		Vec2f Position;
		float Z;
		Vec2f Scale;
		// TODO: Vec3 so that _Z is integrated with LocalPosition

		inline Transform(Vec2f position = { 0.f }, float z = 0.f, Vec2f scale = { 1.f }, Vec2f origin = { 0.f }) noexcept
			: LocalPosition(position), LocalZ(z), LocalScale(scale), Origin(origin),
			Position(position), Z(z), Scale(scale) {}
	};

	struct Hidden
	{
		bool Visible = false;
	};

	struct Style
	{
		sf::Color FillColor = sf::Color::White;
		sf::Color OutlineColor = sf::Color::Transparent;
		float Outline = 0.f;
	};

	struct RectRender
	{
		Vec2f Size{};
		float Corner = 0.f;
	};

	struct CircleRender
	{
		float Radius = 0.f;
	};

	struct TextureRender
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
