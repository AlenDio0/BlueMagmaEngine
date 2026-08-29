#pragma once
#include "Core/Vec2.hpp"
#include "EventDispatcher.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <ranges>

namespace BM
{
	namespace WindowStyle
	{
		enum
		{
			None = sf::Style::None,
			Titlebar = sf::Style::Titlebar,
			Resize = sf::Style::Resize,
			Close = sf::Style::Close,

			Default = sf::Style::Default
		};
	}

	enum class WindowState : uint8_t
	{
		Windowed = static_cast<uint8_t>(sf::State::Windowed),
		Fullscreen = static_cast<uint8_t>(sf::State::Fullscreen)
	};

	struct WindowMode
	{
		Vec2u Size;
		uint32_t BitsPerPixel = 32u;

		WindowMode() noexcept;
		WindowMode(Vec2u size, uint32_t bitsPerPixel = 32u) noexcept;
		WindowMode(const sf::VideoMode& videoMode) noexcept;

		static WindowMode GetDesktopMode() noexcept;
		static const std::vector<WindowMode>& GetFullscreenModes() noexcept;
	};

	struct WindowContext
	{
		WindowMode InitialMode;
		uint32_t InitialStyle = WindowStyle::Default;
		WindowState InitialState = WindowState::Windowed;

		std::string Title = "BlueMagma Window";

		uint32_t FPSLimit = 0u;
		bool VSync = false;

		std::filesystem::path IconPath = "";

		using EventCallbackFn = std::function<void(Event&)>;
		EventCallbackFn EventCallback;
	};
}
