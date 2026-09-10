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
		BM_CORE_DEBUG_FN_ARGS(Context.InitialMode.Size, Context.InitialMode.BitsPerPixel);
		BM_CORE_DEBUG_FN_ARGS(Context.InitialStyle, static_cast<uint8_t>(Context.InitialState));
		BM_CORE_DEBUG_FN_ARGS(Context.SavePositionMemoryOnClose, Context.UsePositionMemoryOnOpen);

		if (!m_Handle)
			m_Handle = std::make_shared<WindowHandle>();

		if (IsOpen())
			Close();

		// TODO: Add in WindowContext a wrapper of sf::ContextSettings to apply in Window::Create() and possibly in Window::ApplyContext()

		m_Handle->create(sf::VideoMode(Context.InitialMode.Size, Context.InitialMode.BitsPerPixel), {},
			Context.InitialStyle, static_cast<sf::State>(Context.InitialState));

		if (!m_Renderer)
			m_Renderer = std::make_shared<Renderer>(m_Handle);

		ApplyContext();

		if (Context.UsePositionMemoryOnOpen && m_WindowPosition != Vec2i(-1, -1))
			SetPosition(m_WindowPosition);

		BM_CORE_INFO_FN("Window created");
	}

	void BM::Window::Destroy() noexcept
	{
		BM_CORE_DEBUG_FN("Window is being destroyed");

		Close();

		m_Renderer.reset();
		m_Handle.reset();

		BM_CORE_INFO_FN("Window destroyed");
	}

	void Window::ReplaceContext(const WindowContext& context) noexcept
	{
		BM_CORE_FN("Replacing window context");

		Context = context;
		ApplyContext();
	}

	void Window::ApplyContext() noexcept
	{
		BM_CORE_FN("Applying window context");

		SetTitle(Context.Title);
		SetFPSLimit(Context.FPSLimit);
		SetVSync(Context.VSync);
		SetIconFromPath(Context.IconPath);
	}

	void BM::Window::Close() const noexcept
	{
		BM_CORE_DEBUG_FN("Window is being closed");

		if (!IsOpen())
		{
			BM_CORE_DEBUG_FN("Window was already closed, nothing changed");
			return;
		}
		else if (Context.SavePositionMemoryOnClose)
		{
			m_WindowPosition = GetPosition();
		}

		GetHandleRef()->close();

		BM_CORE_INFO_FN("Window closed");
	}

	void Window::PollEvents() const noexcept
	{
		if (!Context.EventCallback)
			return;

		while (auto ev = GetHandleRef()->pollEvent())
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
		BM_CORE_DEBUG_FN_ARGS(active);
		return GetHandleRef()->setActive(active);
	}

	void Window::RequestFocus() const noexcept
	{
		BM_CORE_DEBUG_FN("Window is requesting focus");
		GetHandleRef()->requestFocus();
	}

	void Window::SetMousePosition(Vec2i point) const noexcept
	{
		BM_CORE_FN_ARGS(point);
		sf::Mouse::setPosition(point, *GetHandleRef());
	}

	void Window::SetSize(Vec2u size) const noexcept
	{
		BM_CORE_FN_ARGS(size);
		GetHandleRef()->setSize(size);
	}

	void Window::SetTitle(const std::string& title) noexcept
	{
		BM_CORE_FN_ARGS(title);

		Context.Title = title;
		GetHandleRef()->setTitle(title);
	}

	void Window::SetFPSLimit(uint32_t fps) noexcept
	{
		BM_CORE_FN_ARGS(fps);

		Context.FPSLimit = fps;
		GetHandleRef()->setFramerateLimit(fps);
	}

	void Window::SetVSync(bool vsync) noexcept
	{
		BM_CORE_FN_ARGS(vsync);

		Context.VSync = vsync;
		GetHandleRef()->setVerticalSyncEnabled(vsync);
	}

	void Window::SetIconFromPath(const std::filesystem::path& iconPath) noexcept
	{
		BM_CORE_FN_ARGS(iconPath.string());

		Context.IconPath = iconPath;

		sf::Image icon;
		if (iconPath.empty() || !icon.loadFromFile(iconPath))
		{
			BM_CORE_WARN_FN_ARGS(iconPath.string());
			BM_CORE_WARN_FN("Icon path is empty or invalid, nothing changed");
			return;
		}

		SetIcon(icon);
	}

	void Window::SetIcon(const sf::Image& icon) const noexcept
	{
		BM_CORE_FN("Setting icon from image");
		GetHandleRef()->setIcon(icon);
	}

	void Window::SetPosition(Vec2i point) const noexcept
	{
		BM_CORE_FN_ARGS(point);
		GetHandleRef()->setPosition(point);
	}

	bool Window::IsOpen() const noexcept
	{
		return m_Handle && GetHandleRef()->isOpen();
	}

	bool Window::HasFocus() const noexcept
	{
		return GetHandleRef()->hasFocus();
	}

	Vec2i Window::GetMousePosition() const noexcept
	{
		return sf::Mouse::getPosition(*m_Handle);
	}

	Vec2u Window::GetSize() const noexcept
	{
		return GetHandleRef()->getSize();
	}

	Vec2i Window::GetPosition() const noexcept
	{
		return GetHandle().lock()->getPosition();
	}

	std::weak_ptr<Renderer> Window::GetRenderer() noexcept
	{
		return m_Renderer;
	}

	std::weak_ptr<WindowHandle> Window::GetHandle() const noexcept
	{
		return GetHandleRef();
	}

	std::shared_ptr<WindowHandle> Window::GetHandleRef() const noexcept
	{
		BM_CORE_ASSERT(m_Handle != nullptr, "Window Handle not created");
		return m_Handle;
	}
}