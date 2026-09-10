#include "bmpch.hpp"
#include "Application.hpp"

#include "Core/Timer.hpp"

#include <algorithm>

namespace BM
{
	Application::Application(const ApplicationContext& appContext) noexcept
		: m_Context(appContext)
	{
		BM_CORE_DEBUG_FN_ARGS(appContext.TPSLimit, appContext.MaxLagTime, appContext.TimeScale);
		BM_CORE_DEBUG_FN_ARGS(appContext.DefaultWindowCloseEvent, appContext.StopOnWindowCloseEvent);
	}

	Application::~Application() noexcept
	{
		BM_CORE_INFO_FN("Application is being destroyed");
	}

	void Application::SetDefaultWindowCloseEvent(bool flag) noexcept
	{
		BM_CORE_FN_ARGS(flag);
		m_Context.DefaultWindowCloseEvent = flag;
	}

	void Application::SetStopOnWindowCloseEvent(bool flag) noexcept
	{
		BM_CORE_FN_ARGS(flag);
		m_Context.StopOnWindowCloseEvent = flag;
	}

	void Application::SetTPSLimit(uint32_t tps) noexcept
	{
		BM_CORE_FN_ARGS(tps);
		m_Context.TPSLimit = tps;
	}

	void Application::SetMaxLagTime(float lag) noexcept
	{
		BM_CORE_FN_ARGS(lag);
		m_Context.MaxLagTime = lag;
	}

	void Application::SetTimeScale(float timeScale) noexcept
	{
		BM_CORE_FN_ARGS(timeScale);
		m_Context.TimeScale = timeScale;
	}

	const ApplicationContext& Application::GetContext() const noexcept
	{
		return m_Context;
	}

	void Application::Run()
	{
		BM_CORE_INFO_FN("Application is starting");

		m_Running = true;

		const float cTimeStep = 1.f / static_cast<float>(m_Context.TPSLimit);
		float timeAccumulator = 0.f;

		Timer timer;
		while (m_Running)
		{
			if (!Layers.ProcessLayerChanges())
			{
				Stop();
				break;
			}

			if (m_Window)
				m_Window->PollEvents();

			const auto& layers = Layers.GetLayers();

			float unscaledDeltaTime = std::min(timer.Restart().AsSeconds(), m_Context.MaxLagTime);
			float deltaTime = unscaledDeltaTime * m_Context.TimeScale;
			timeAccumulator += deltaTime;

			while (timeAccumulator >= cTimeStep)
			{
				timeAccumulator -= cTimeStep;
				for (const auto& layer : layers)
					layer->OnTick(cTimeStep);
			}

			for (const auto& layer : layers)
				layer->OnUpdate(deltaTime);

			std::shared_ptr<Renderer> renderer = GetRenderer().lock();

			if (renderer)
				renderer->Clear();

			for (const auto& layer : layers)
				layer->OnRender();

			if (renderer)
				renderer->Display();
		}

		Layers.Clear();
	}

	void Application::Stop()
	{
		BM_CORE_INFO_FN("Application is stopping");
		m_Running = false;
	}

	void Application::CreateOrReplaceWindow(WindowContext windowContext) noexcept
	{
		BM_CORE_DEBUG_FN("Creating or replacing a window with a new window context");

		if (!windowContext.EventCallback)
			windowContext.EventCallback = [&](Event& event) { EventCallback(event); };

		if (m_Window)
			m_Window->Context = windowContext;
		else
			m_Window = std::make_unique<Window>(windowContext);

		m_Window->Create();
	}

	std::weak_ptr<Window> Application::GetWindow() noexcept
	{
		return m_Window;
	}

	std::weak_ptr<Renderer> Application::GetRenderer() noexcept
	{
		if (m_Window)
			return m_Window->GetRenderer();

		return {};
	}

	void Application::EventCallback(Event& event) noexcept
	{
		if (m_Context.DefaultWindowCloseEvent || m_Context.StopOnWindowCloseEvent)
		{
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<EventHandle::Closed>(BM_EVENT_FN(OnCloseEvent));
		}

		for (const auto& layer : Layers.GetLayers() | std::views::reverse)
		{
			if (event.Done)
				break;

			layer->OnEvent(event);
		}
	}

	bool Application::OnCloseEvent(const EventHandle::Closed& event) noexcept
	{
		if (m_Context.DefaultWindowCloseEvent)
			m_Window->Close();

		if (m_Context.StopOnWindowCloseEvent)
			Stop();

		return false;
	}
}
