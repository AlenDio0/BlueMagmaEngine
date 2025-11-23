#include "bmpch.hpp"
#include "Window.hpp"

namespace BM
{
	Window::Window(const WindowContext& context) noexcept
		: m_Context(context)
	{
	}

	Window::~Window() noexcept
	{
		Destroy();
	}

	void BM::Window::Create() noexcept
	{
		try
		{
			m_Handle = std::make_unique<sf::RenderWindow>();

			m_Handle->create(sf::VideoMode({ m_Context._Width, m_Context._Height }), m_Context._Title, m_Context._Style);

			m_Handle->setFramerateLimit(m_Context._FPSLimit);
			m_Handle->setVerticalSyncEnabled(m_Context._VSync);
		}
		catch (...) {}
	}

	void BM::Window::Destroy() noexcept
	{
		m_Handle->close();
		m_Handle.reset();
	}

	void BM::Window::Close() noexcept
	{
		try
		{
			m_Handle->close();
		}
		catch (...) {}
	}

	void Window::PollEvent() noexcept
	{
		if (!m_Context._EventCallback)
			return;

		while (auto ev = m_Handle->pollEvent())
		{
			Event event(static_cast<EventHandle>(ev.value()));
			m_Context._EventCallback(event);
		}
	}

	void Window::ClearScreen(sf::Color newColor) noexcept
	{
		try
		{
			m_Handle->clear(newColor);
		}
		catch (...) {}
	}

	void Window::Draw(const sf::Drawable& drawable) noexcept
	{
		try
		{
			m_Handle->draw(drawable);
		}
		catch (...) {}
	}

	void Window::DisplayScreen() noexcept
	{
		try
		{
			m_Handle->display();
		}
		catch (...) {}
	}

	bool Window::IsOpen() const noexcept
	{
		try
		{
			return m_Handle && m_Handle->isOpen();
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	sf::Vector2u Window::GetSize() const noexcept
	{
		return m_Handle->getSize();
	}

	sf::RenderWindow& Window::GetHandle() const noexcept
	{
		return *m_Handle;
	}
}