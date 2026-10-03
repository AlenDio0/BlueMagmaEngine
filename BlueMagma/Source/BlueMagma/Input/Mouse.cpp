#include "bmpch.hpp"
#include "Mouse.hpp"

namespace BM::Mouse
{
	bool IsPressed(Button button) noexcept
	{
		return sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(button));
	}

	Vec2i GetPosition() noexcept
	{
		return sf::Mouse::getPosition();
	}

	Vec2i GetPosition(const Window& window) noexcept
	{
		if (auto handle = window.GetHandle().lock())
			return sf::Mouse::getPosition(*handle);

		return GetPosition();
	}

	void SetPosition(Vec2i position) noexcept
	{
		sf::Mouse::setPosition(position);
	}

	void SetPosition(Vec2i position, const Window& window) noexcept
	{
		if (auto handle = window.GetHandle().lock())
			sf::Mouse::setPosition(position, *handle);
	}
}

