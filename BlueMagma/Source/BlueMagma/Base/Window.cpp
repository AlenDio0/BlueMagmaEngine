#include "bmpch.hpp"
#include "Window.hpp"
#include <SFML/Graphics/Image.hpp>

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
		BM_CORE_DEBUG("{}()\n - Size: {}\n - Title: '{}'\n - Style: {}\n - FPSLimit: {}\n - VSync: {}\n - IconPath: '{}'", __FUNCTION__,
			m_Context.Size, m_Context.Title, m_Context.Style, m_Context.FPSLimit, m_Context.VSync, m_Context.IconPath.string());

		m_Handle = std::make_unique<sf::RenderWindow>();
		m_Renderer = std::make_unique<Renderer>(*m_Handle);

		m_Handle->create(sf::VideoMode(m_Context.Size), m_Context.Title, m_Context.Style);

		m_Handle->setFramerateLimit(m_Context.FPSLimit);
		m_Handle->setVerticalSyncEnabled(m_Context.VSync);

		if (!m_Context.IconPath.empty())
		{
			sf::Image icon;
			if (icon.loadFromFile(m_Context.IconPath))
				m_Handle->setIcon(icon);
		}

		BM_CORE_INFO("Window created");
	}

	void BM::Window::Destroy() noexcept
	{
		BM_CORE_FN();

		m_Handle->close();

		m_Renderer.reset();
		m_Handle.reset();


		BM_CORE_INFO("Window destroyed");
	}

	void BM::Window::Close() noexcept
	{
		BM_CORE_FN();

		m_Handle->close();

		BM_CORE_INFO("Window closed");
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

	bool Window::SetActive(bool active) noexcept
	{
		return m_Handle->setActive(active);
	}

	void Window::RequestFocus() noexcept
	{
		m_Handle->requestFocus();
	}

	bool Window::IsOpen() const noexcept
	{
		return m_Handle && m_Handle->isOpen();
	}

	bool Window::HasFocus() const noexcept
	{
		return m_Handle->hasFocus();
	}

	const WindowContext& Window::GetContext() const noexcept
	{
		return m_Context;
	}

	Vec2u Window::GetSize() const noexcept
	{
		return m_Handle->getSize();
	}

	Renderer& Window::GetRenderer() noexcept
	{
		return *m_Renderer;
	}

	sf::RenderWindow& Window::GetHandle() const noexcept
	{
		BM_CORE_ASSERT(m_Handle != nullptr, "Window Handle not contructed yet");
		return *m_Handle;
	}
}