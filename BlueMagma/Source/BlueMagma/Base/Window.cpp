#include "bmpch.hpp"
#include "Window.hpp"
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace BM
{
	Window::Window(const WindowContext& context) noexcept
		: Context(context)
	{}

	Window::~Window() noexcept
	{
		Destroy();
	}

	void BM::Window::Create() noexcept
	{
		BM_CORE_DEBUG("{}()\n - InitialSize: {}\n - InitialStyle: {}\n - InitialState: {}", __FUNCTION__,
			Context.InitialSize, static_cast<uint8_t>(Context.InitialStyle), static_cast<uint8_t>(Context.InitialState));

		if (!m_Handle)
			m_Handle = std::make_unique<sf::RenderWindow>();
		m_Handle->create(sf::VideoMode(Context.InitialSize), {}, Context.InitialStyle, static_cast<sf::State>(Context.InitialState));

		if (!m_Renderer)
			m_Renderer = std::make_unique<Renderer>(*m_Handle);

		ApplyContext();

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

	void Window::ApplyContext() noexcept
	{
		BM_CORE_DEBUG("{}()\n - Title: '{}'\n - FPSLimit: {}\n - VSync: {}\n - IconPath: '{}'", __FUNCTION__,
			Context.Title, Context.FPSLimit, Context.VSync, Context.IconPath.string());

		SetTitle(Context.Title);
		SetFPSLimit(Context.FPSLimit);
		SetVSync(Context.VSync);
		SetIconFromPath(Context.IconPath);
	}

	void BM::Window::Close() noexcept
	{
		BM_CORE_FN();

		m_Handle->close();

		BM_CORE_INFO("Window closed");
	}

	void Window::PollEvent() noexcept
	{
		if (!Context.EventCallback)
			return;

		while (auto ev = m_Handle->pollEvent())
		{
			Event event = static_cast<EventHandle>(ev.value());
			Context.EventCallback(event);
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

	void Window::SetMousePosition(Vec2i point) noexcept
	{
		sf::Mouse::setPosition(point, *m_Handle);
	}

	void Window::SetSize(Vec2u size) noexcept
	{
		m_Handle->setSize(size);
	}

	void Window::SetTitle(const std::string& title) noexcept
	{
		Context.Title = title;
		m_Handle->setTitle(title);
	}

	void Window::SetFPSLimit(uint32_t fps) noexcept
	{
		Context.FPSLimit = fps;
		m_Handle->setFramerateLimit(fps);
	}

	void Window::SetVSync(bool vsync) noexcept
	{
		Context.VSync = vsync;
		m_Handle->setVerticalSyncEnabled(vsync);
	}

	void Window::SetIconFromPath(const std::filesystem::path& iconPath) noexcept
	{
		Context.IconPath = iconPath;
		if (!iconPath.empty())
		{
			sf::Image icon;
			if (icon.loadFromFile(iconPath))
				SetIcon(icon);
		}
	}

	void Window::SetIcon(const sf::Image& icon) noexcept
	{
		m_Handle->setIcon(icon);
	}

	bool Window::IsOpen() const noexcept
	{
		return m_Handle && m_Handle->isOpen();
	}

	bool Window::HasFocus() const noexcept
	{
		return m_Handle->hasFocus();
	}

	Vec2i Window::GetMousePosition() const noexcept
	{
		return sf::Mouse::getPosition(*m_Handle);
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