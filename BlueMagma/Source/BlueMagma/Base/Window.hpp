#pragma once
#include "Renderer.hpp"
#include "EventDispatcher.hpp"
#include "Core/Vec2.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <memory>
#include <string>
#include <cstdint>
#include <filesystem>
#include <functional>

namespace BM
{
	struct WindowContext
	{
		std::string Title = "BlueMagma Application";
		Vec2u Size = 800;
		uint32_t Style = sf::Style::Default;

		uint32_t FPSLimit = 0;
		bool VSync = true;

		std::filesystem::path IconPath = "";

		using EventCallbackFn = std::function<void(Event&)>;
		EventCallbackFn EventCallback;
	};

	class Window
	{
	public:
		explicit Window(const WindowContext& context = {}) noexcept;
		~Window() noexcept;

		void Create() noexcept;
		void Destroy() noexcept;

		void Close() noexcept;

		void PollEvent() noexcept;

		bool SetActive(bool active) noexcept;
		void RequestFocus() noexcept;

		bool IsOpen() const noexcept;
		bool HasFocus() const noexcept;

		const WindowContext& GetContext() const noexcept;
		Vec2u GetSize() const noexcept;

		Renderer& GetRenderer() noexcept;
		sf::RenderWindow& GetHandle() const noexcept;
	private:
		WindowContext m_Context;

		std::unique_ptr<Renderer> m_Renderer;
		std::unique_ptr<sf::RenderWindow> m_Handle;
	};
}
