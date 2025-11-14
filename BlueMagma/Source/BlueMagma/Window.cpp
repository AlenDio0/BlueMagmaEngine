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

			m_Handle->create(sf::VideoMode({ m_Context._Width, m_Context._Height }),
				m_Context._Title, m_Context._Style
			);
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

	std::optional<sf::Event> Window::PollEvent() const noexcept
	{
		return m_Handle->pollEvent();
	}

	void Window::OnEvent(const sf::Event& event) noexcept
	{
		if (event.is<sf::Event::Closed>())
			Close();

		else if (auto resizedEvent = event.getIf<sf::Event::Resized>())
		{
			sf::View view = m_Handle->getView();

			sf::Vector2f size = static_cast<sf::Vector2f>(resizedEvent->size);
			view.setSize(size);

			m_Handle->setView(view);
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

	sf::RenderWindow& Window::GetHandle() const noexcept
	{
		return *m_Handle;
	}
}