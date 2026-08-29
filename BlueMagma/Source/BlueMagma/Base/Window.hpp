#pragma once
#include "WindowContext.hpp"
#include "Renderer.hpp"
#include "Core/Vec2.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <memory>
#include <string>
#include <cstdint>
#include <filesystem>
#include <functional>

namespace BM
{
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

		void Close() const noexcept;

		void PollEvents() const noexcept;

		void UpdateModeFocus() const noexcept;

		bool SetActive(bool active) const noexcept;
		void RequestFocus() const noexcept;

		void SetSize(Vec2u size) const noexcept;
		void SetTitle(const std::string& title) noexcept;
		void SetFPSLimit(uint32_t fps) noexcept;
		void SetVSync(bool vsync) noexcept;
		void SetIconFromPath(const std::filesystem::path& iconPath) noexcept;
		void SetIcon(const sf::Image& icon) const noexcept;
		void SetPosition(Vec2i point) const noexcept;
		void SetMousePosition(Vec2i point) const noexcept;

		bool IsOpen() const noexcept;
		bool HasFocus() const noexcept;

		Vec2u GetSize() const noexcept;
		Vec2i GetPosition() const noexcept;
		Vec2i GetMousePosition() const noexcept;

		Renderer& GetRenderer() noexcept;
		sf::RenderWindow& GetHandle() const noexcept;
	private:
		std::unique_ptr<Renderer> m_Renderer;
		std::unique_ptr<sf::RenderWindow> m_Handle;
	};
}
