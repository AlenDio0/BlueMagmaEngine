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
		BM_CORE_DEBUG("{}()\n - Size: {}\n - Title: {}\n - Style: {}\n - FPSLimit: {}\n - VSync: {}", __FUNCTION__,
			m_Context.Size, m_Context.Title, m_Context.Style, m_Context.FPSLimit, m_Context.VSync);

		try
		{
			m_Handle = std::make_unique<sf::RenderWindow>();
			m_Handle->create(sf::VideoMode(m_Context.Size), m_Context.Title, m_Context.Style);

			m_Handle->setFramerateLimit(m_Context.FPSLimit);
			m_Handle->setVerticalSyncEnabled(m_Context.VSync);

			BM_CORE_INFO("Window created");
		}
		catch (...) {}
	}

	void BM::Window::Destroy() noexcept
	{
		BM_CORE_FN();

		m_Handle->close();
		m_Handle.reset();

		BM_CORE_INFO("Window destroyed");
	}

	void BM::Window::Close() noexcept
	{
		BM_CORE_FN();

		try
		{
			m_Handle->close();

			BM_CORE_INFO("Window closed");
		}
		catch (...) {}
	}

	void Window::PollEvent() noexcept
	{
		if (!m_Context.EventCallback)
			return;

		while (auto ev = m_Handle->pollEvent())
		{
			Event event(static_cast<EventHandle>(ev.value()));
			m_Context.EventCallback(event);
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

	Vec2u Window::GetSize() const noexcept
	{
		return m_Handle->getSize();
	}

	sf::RenderWindow& Window::GetHandle() const noexcept
	{
		BM_CORE_ASSERT(m_Handle != nullptr, "Window Handle not contructed yet");

		return *m_Handle;
	}
}