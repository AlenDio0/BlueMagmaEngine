#pragma once
#include "BlueMagma/Math/Vec2.hpp"
#include "BlueMagma/Graphics/Window.hpp"

#include <SFML/Window/Mouse.hpp>

namespace BM::Mouse
{
	using Button = sf::Mouse::Button;
	using Wheel = sf::Mouse::Wheel;

	[[nodiscard]] bool IsPressed(Button button) noexcept;

	[[nodiscard]] Vec2i GetPosition() noexcept;
	[[nodiscard]] Vec2i GetPosition(const Window& window) noexcept;

	void SetPosition(Vec2i position) noexcept;
	void SetPosition(Vec2i position, const Window& window) noexcept;
}
