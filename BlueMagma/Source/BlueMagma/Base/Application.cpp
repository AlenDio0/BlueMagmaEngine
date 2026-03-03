#include "bmpch.hpp"
#include "Application.hpp"
#include "Core/Timer.hpp"

namespace BM
{
	Application::Application(const ApplicationContext& context) noexcept
		: m_Context(context)
	{
		BM_CORE_DEBUG("{}()\n - DefaultWindowCloseEvent: {}\n - DefaultWindowResizeEvent: {}", __FUNCTION__,
			context.DefaultWindowCloseEvent, context.DefaultWindowResizeEvent);

		s_Instance = this;

		if (!m_Context.Window.EventCallback)
			m_Context.Window.EventCallback = [&](Event& event) { EventCallback(event); };

		m_Window = std::make_unique<Window>(m_Context.Window);
		m_Window->Create();
	}

	Application::~Application() noexcept
	{
		s_Instance = nullptr;
	}

	Application& Application::Get() noexcept
	{
		BM_CORE_ASSERT(s_Instance != nullptr, "Application not constructed yet");

		return *s_Instance;
	}

	void Application::Run()
	{
		BM_CORE_FN();

		m_Running = true;

		Timer timer;
		while (m_Running)
		{
			if (!m_Machine.ProcessLayerChanges())
			{
				Stop();
				break;
			}

			m_Window->PollEvent();

			if (!m_Window->IsOpen())
				Stop();

			const auto& layers = m_Machine.GetLayers();

			float deltaTime = timer.Restart().AsSeconds();
			for (const auto& layer : layers)
				layer->OnUpdate(deltaTime);

			m_Window->GetRenderer().Clear();

			for (const auto& layer : layers)
				layer->OnRender();

			m_Window->GetRenderer().Display();
		}

		m_Machine.Clear();
	}

	void Application::Stop()
	{
		BM_CORE_FN();

		m_Running = false;
	}

	Window& Application::GetWindow() noexcept
	{
		BM_CORE_ASSERT(m_Window != nullptr, "Window not contructed yet");

		return *m_Window;
	}

	Renderer& Application::GetRenderer() noexcept
	{
		return GetWindow().GetRenderer();
	}

	LayerMachine& Application::GetMachine() noexcept
	{
		return m_Machine;
	}

	AssetManager& Application::GetAssets() noexcept
	{
		return m_Assets;
	}

	void Application::EventCallback(Event& event) noexcept
	{
		{
			EventDispatcher dispatcher(event);

			if (m_Context.DefaultWindowCloseEvent)
				dispatcher.Dispatch<EventHandle::Closed>(BM_EVENT_FN(OnCloseEvent));
			if (m_Context.DefaultWindowResizeEvent)
				dispatcher.Dispatch<EventHandle::Resized>(BM_EVENT_FN(OnResizeEvent));
		}

		for (const auto& layer : m_Machine.GetLayers() | std::views::reverse)
		{
			if (event.Done)
				break;

			layer->OnEvent(event);
		}
	}

	bool Application::OnCloseEvent(const EventHandle::Closed& event) noexcept
	{
		m_Window->Close();

		return false;
	}

	bool Application::OnResizeEvent(const EventHandle::Resized& event) noexcept
	{
		Renderer& renderer = m_Window->GetRenderer();

		sf::View view = renderer.GetView();
		view.setSize(m_Window->GetSize());
		renderer.SetView(view);

		return false;
	}
}
