#include "bmpch.hpp"
#include "Window.hpp"
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace BM
{
	Window::Window(const WindowContext& context) noexcept
		: Context(context)
	{
	}

	Window::~Window() noexcept
	{
		Destroy();
	}

	void BM::Window::Create() noexcept
	{
		BM_CORE_DEBUG("{}()\n - InitialStyle: {}\n - InitialState: {}", __FUNCTION__,
			static_cast<uint8_t>(Context.InitialStyle), static_cast<uint8_t>(Context.InitialState));

		if (!m_Handle)
			m_Handle = std::make_unique<sf::RenderWindow>();

		Close();

		m_Handle->create(sf::VideoMode(Context.InitialMode.Size, Context.InitialMode.BitsPerPixel), {}, Context.InitialStyle, static_cast<sf::State>(Context.InitialState));

		if (!m_Renderer)
			m_Renderer = std::make_unique<Renderer>(*m_Handle);

		ApplyContext();

		BM_CORE_INFO("Window created");
	}

	void BM::Window::Destroy() noexcept
	{
		BM_CORE_FN();

		Close();

		m_Renderer.reset();
		m_Handle.reset();

		BM_CORE_INFO("Window destroyed");
	}

	void Window::ReplaceContext(const WindowContext& context) noexcept
	{
		Context = context;
		ApplyContext();
	}

	void Window::ApplyContext() noexcept
	{
		BM_CORE_FN();

		SetTitle(Context.Title);
		SetFPSLimit(Context.FPSLimit);
		SetVSync(Context.VSync);
		SetIconFromPath(Context.IconPath);
	}

	void BM::Window::Close() const noexcept
	{
		BM_CORE_FN();

		if (!IsOpen())
		{
			BM_CORE_DEBUG("Window was already closed, nothing changed");
			return;
		}

		GetHandle().close();

		BM_CORE_INFO("Window closed");
	}

	void Window::PollEvents() const noexcept
	{
		if (!Context.EventCallback)
			return;

		while (auto ev = GetHandle().pollEvent())
		{
			Event event = static_cast<EventHandle>(ev.value());
			Context.EventCallback(event);
		}
	}

	void Window::UpdateModeFocus() const noexcept
	{
		if (!m_FocusState && HasFocus())
		{
			SetSize(Context.InitialMode.Size);
			m_FocusState = true;
		}
		else if (m_FocusState && !HasFocus())
		{
			SetSize(BM::Vec2i::Zero());
			m_FocusState = false;
		}
	}

	bool Window::SetActive(bool active) const noexcept
	{
		BM_CORE_FN("active: {}", active);

		return GetHandle().setActive(active);
	}

	void Window::RequestFocus() const noexcept
	{
		BM_CORE_FN();

		GetHandle().requestFocus();
	}

	void Window::SetMousePosition(Vec2i point) const noexcept
	{
		BM_CORE_FN("point: {}", point);

		sf::Mouse::setPosition(point, GetHandle());
	}

	void Window::SetSize(Vec2u size) const noexcept
	{
		BM_CORE_FN("size: {}", size);

		GetHandle().setSize(size);
	}

	void Window::SetTitle(const std::string& title) noexcept
	{
		BM_CORE_FN("title: '{}'", title);

		Context.Title = title;
		GetHandle().setTitle(title);
	}

	void Window::SetFPSLimit(uint32_t fps) noexcept
	{
		BM_CORE_FN("fps: {}", fps);

		Context.FPSLimit = fps;
		GetHandle().setFramerateLimit(fps);
	}

	void Window::SetVSync(bool vsync) noexcept
	{
		BM_CORE_FN("vsync: {}", vsync);

		Context.VSync = vsync;
		GetHandle().setVerticalSyncEnabled(vsync);
	}

	void Window::SetIconFromPath(const std::filesystem::path& iconPath) noexcept
	{
		BM_CORE_FN("iconPath: '{}'", iconPath.string());

		Context.IconPath = iconPath;

		sf::Image icon;
		if (iconPath.empty() || !icon.loadFromFile(iconPath))
		{
			BM_CORE_DEBUG("Icon path is empty or invalid, nothing changed");
			return;
		}

		SetIcon(icon);
	}

	void Window::SetIcon(const sf::Image& icon) const noexcept
	{
		BM_CORE_FN();

		GetHandle().setIcon(icon);
	}

	void Window::SetPosition(Vec2i point) const noexcept
	{
		BM_CORE_FN();

		GetHandle().setPosition(point);
	}

	bool Window::IsOpen() const noexcept
	{
		return m_Handle && GetHandle().isOpen();
	}

	bool Window::HasFocus() const noexcept
	{
		return GetHandle().hasFocus();
	}

	Vec2i Window::GetMousePosition() const noexcept
	{
		return sf::Mouse::getPosition(*m_Handle);
	}

	Vec2u Window::GetSize() const noexcept
	{
		return GetHandle().getSize();
	}

	Vec2i Window::GetPosition() const noexcept
	{
		return GetHandle().getPosition();
	}

	Renderer& Window::GetRenderer() noexcept
	{
		return *m_Renderer;
	}

	sf::RenderWindow& Window::GetHandle() const noexcept
	{
		BM_CORE_ASSERT(m_Handle != nullptr, "Window Handle not created");
		return *m_Handle;
	}
}