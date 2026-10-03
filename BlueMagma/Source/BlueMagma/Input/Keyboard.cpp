#include "bmpch.hpp"
#include "Keyboard.hpp"

#include <SFML/System/String.hpp>

namespace BM::Keyboard
{
	bool IsPressed(Key key) noexcept
	{
		return sf::Keyboard::isKeyPressed(key);
	}

	bool IsPressed(Scancode scancode) noexcept
	{
		return sf::Keyboard::isKeyPressed(scancode);
	}

	Key ScancodeToKey(Scancode scancode) noexcept
	{
		return sf::Keyboard::localize(scancode);
	}

	Scancode KeyToScancode(Key key) noexcept
	{
		return sf::Keyboard::delocalize(key);
	}

	std::string GetDescription(Scancode scancode) noexcept
	{
		return sf::Keyboard::getDescription(scancode).toAnsiString();
	}

	void SetVirtualKeyboardVisible(bool visible) noexcept
	{
		sf::Keyboard::setVirtualKeyboardVisible(visible);
	}
}
