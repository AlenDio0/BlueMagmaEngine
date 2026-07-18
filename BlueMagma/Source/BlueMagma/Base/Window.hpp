#pragma once
#include "Renderer.hpp"
#include "EventDispatcher.hpp"
#include "Core/Vec2.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <SFML/Graphics/Image.hpp>
#include <memory>
#include <string>
#include <cstdint>
#include <filesystem>
#include <functional>

namespace BM
{
	namespace WindowEnum
	{
		namespace Style
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

		enum class State : uint8_t
		{
			Windowed = sf::State::Windowed,
			Fullscreen = sf::State::Fullscreen
		};
	}

	struct WindowContext
	{
		Vec2u InitialSize = Vec2u(800u);
		uint32_t InitialStyle = WindowEnum::Style::Default;
		WindowEnum::State InitialState = WindowEnum::State::Windowed;

		std::string Title = "BlueMagma Application";

		uint32_t FPSLimit = 0u;
		bool VSync = false;

		std::filesystem::path IconPath = "";

		using EventCallbackFn = std::function<void(Event&)>;
		EventCallbackFn EventCallback;
	};

	class Window
	{
	public:
		WindowContext Context;
	public:
		explicit Window(const WindowContext& context = {}) noexcept;
		~Window() noexcept;

		void Create() noexcept;
		void Destroy() noexcept;

		void ApplyContext() noexcept;

		void Close() noexcept;

		void PollEvent() noexcept;

		bool SetActive(bool active) noexcept;
		void RequestFocus() noexcept;

		void SetMousePosition(Vec2i point) noexcept;
		void SetSize(Vec2u size) noexcept;
		void SetTitle(const std::string& title) noexcept;
		void SetFPSLimit(uint32_t fps) noexcept;
		void SetVSync(bool vsync) noexcept;
		void SetIconFromPath(const std::filesystem::path& iconPath) noexcept;
		void SetIcon(const sf::Image& icon) noexcept;

		bool IsOpen() const noexcept;
		bool HasFocus() const noexcept;

		Vec2i GetMousePosition() const noexcept;
		Vec2u GetSize() const noexcept;

		Renderer& GetRenderer() noexcept;
		sf::RenderWindow& GetHandle() const noexcept;
	private:
		std::unique_ptr<Renderer> m_Renderer;
		std::unique_ptr<sf::RenderWindow> m_Handle;
	};
}
