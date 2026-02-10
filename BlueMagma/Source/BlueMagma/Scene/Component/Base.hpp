#pragma once
#include "Core/Vec2.hpp"
#include "Asset/Asset.hpp"
#include "Core/Rect.hpp"
#include <SFML/Graphics/Color.hpp>
#include <string>
#include <cstdint>

namespace BM::Component
{
	struct Transform
	{
		Vec2f _LocalPosition;
		float _LocalZ;
		Vec2f _LocalScale;

		Vec2f _Position;
		float _Z;
		Vec2f _Scale;
		Vec2f _Origin;
		// TODO: Vec3 so that _Z is integrated with LocalPosition

		inline Transform(Vec2f position = { 0.f }, float z = 0.f, Vec2f scale = { 1.f }, Vec2f origin = { 0.f }) noexcept
			: _LocalPosition(position), _LocalZ(z), _LocalScale(scale), _Origin(origin),
			_Position(position), _Z(z), _Scale(scale) {
		}
	};

	struct Style
	{
		sf::Color _FillColor = sf::Color::White;
		sf::Color _OutlineColor = sf::Color::Transparent;
		float _Outline = 0.f;
	};

	struct RectRender
	{
		Vec2f _Size{};
		float _Corner = 0.f;
	};

	struct CircleRender
	{
		float _Radius = 0.f;
	};

	struct TextureRender
	{
		const Texture* _TexturePtr;
		RectInt _TextureRect{};
		bool _UseRect = false;

		inline TextureRender(const Texture* texture = nullptr) noexcept
			: _TexturePtr(texture) {
		}
		inline TextureRender(const Texture* texture, RectInt rect) noexcept
			: _TexturePtr(texture), _TextureRect(rect), _UseRect(true) {
		}
	};

	struct TextRender
	{
		const Font* _FontPtr = nullptr;
		std::string _Content{ "" };
		uint32_t _CharSize = 30u;
	};
}
