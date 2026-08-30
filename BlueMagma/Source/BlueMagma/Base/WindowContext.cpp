#include "bmpch.hpp"
#include "WindowContext.hpp"

namespace BM
{
	WindowMode::WindowMode() noexcept
		: WindowMode(GetDesktopMode())
	{
	}

	WindowMode::WindowMode(Vec2u size, uint32_t bitsPerPixel) noexcept
		: Size(size), BitsPerPixel(bitsPerPixel)
	{
	}

	WindowMode::WindowMode(const sf::VideoMode& videoMode) noexcept
		: Size(videoMode.size), BitsPerPixel(videoMode.bitsPerPixel)
	{
	}

	WindowMode WindowMode::GetDesktopMode() noexcept
	{
		auto mode = sf::VideoMode::getDesktopMode();
		mode.size.y += 1u;
		return mode;
	}

	inline static std::vector<WindowMode> GetFullscreenModesImpl() noexcept {
		auto fullscreenModes = sf::VideoMode::getFullscreenModes() | std::views::transform([](auto mode) { return WindowMode(mode); });
		return std::vector<WindowMode>(fullscreenModes.begin(), fullscreenModes.end());
	}

	const std::vector<WindowMode>& WindowMode::GetFullscreenModes() noexcept
	{
		static const std::vector<WindowMode> sFullscreenModes = GetFullscreenModesImpl();
		return sFullscreenModes;
	}
}
