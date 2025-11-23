#pragma once
#include "EventDispatcher.hpp"
#include <string>
#include <cstdint>
#include <functional>
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>

namespace BM
{
	struct WindowContext
	{
		std::string _Title = "BlueMagma Application";
		uint32_t _Width = 800, _Height = 800;
		uint32_t _Style = sf::Style::Default;

		uint32_t _FPSLimit = 0;
		bool _VSync = true;

		using EventCallbackFn = std::function<void(Event&)>;
		EventCallbackFn _EventCallback;
	};

	class Window
	{
	public:
		Window(const WindowContext& context = {}) noexcept;
		~Window() noexcept;

		void Create() noexcept;
		void Destroy() noexcept;

		void Close() noexcept;

		void PollEvent() noexcept;

		void ClearScreen(sf::Color newColor = {}) noexcept;
		void Draw(const sf::Drawable& drawable) noexcept;
		void DisplayScreen() noexcept;

		bool IsOpen() const noexcept;

		sf::Vector2u GetSize() const noexcept;

		sf::RenderWindow& GetHandle() const noexcept;
	private:
		WindowContext m_Context;
		std::unique_ptr<sf::RenderWindow> m_Handle;
	};
}
