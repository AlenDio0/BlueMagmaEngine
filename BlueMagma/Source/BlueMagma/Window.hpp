#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <optional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>

namespace BM
{
	struct WindowContext
	{
		std::string _Title = "BlueMagma Application";
		uint32_t _Width = 800, _Height = 800;
		uint32_t _Style = sf::Style::Default;
	};

	class Window
	{
	public:
		Window(const WindowContext& context = {}) noexcept;
		~Window() noexcept;

		void Create() noexcept;
		void Destroy() noexcept;

		void Close() noexcept;

		std::optional<sf::Event> PollEvent() const noexcept;
		void OnEvent(const sf::Event& event) noexcept;

		void ClearScreen(sf::Color newColor = {}) noexcept;
		void Draw(const sf::Drawable& drawable) noexcept;
		void DisplayScreen() noexcept;

		bool IsOpen() const noexcept;

		sf::RenderWindow& GetHandle() const noexcept;
	private:
		WindowContext m_Context;
		std::unique_ptr<sf::RenderWindow> m_Handle;
	};
}
