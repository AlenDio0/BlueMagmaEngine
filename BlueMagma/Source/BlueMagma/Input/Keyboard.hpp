#pragma once

#include <SFML/Window/Keyboard.hpp>

#include <string>

namespace BM::Keyboard
{
	using Key = sf::Keyboard::Key;
	using Scancode = sf::Keyboard::Scancode;

	[[nodiscard]] bool IsPressed(Key key) noexcept;
	[[nodiscard]] bool IsPressed(Scancode scancode) noexcept;

	[[nodiscard]] Key ScancodeToKey(Scancode scancode) noexcept;
	[[nodiscard]] Scancode KeyToScancode(Key key) noexcept;

	[[nodiscard]] std::string GetDescription(Scancode scancode) noexcept;

	void SetVirtualKeyboardVisible(bool visible) noexcept;
}
