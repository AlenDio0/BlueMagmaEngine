#pragma once
#include "Vec2.hpp"
#include "Rect.hpp"
#include <SFML/Graphics/Transform.hpp>

namespace BM
{
	struct Transform2D
	{
		Vec2f Position{ 0.f };
		Vec2f Scale{ 1.f };
		Vec2f Origin{ 0.f };
		float Rotation = 0.f;

		static constexpr sf::Transform ToMatrix(const Transform2D& transform, RectFloat box) noexcept {
			sf::Transform matrix;
			return matrix
				.translate(transform.Position)
				.rotate(sf::degrees(transform.Rotation))
				.scale(transform.Scale)
				.translate((box.Position + (transform.Origin * box.Size).Round()) * -1.f);
		}
	};
}
