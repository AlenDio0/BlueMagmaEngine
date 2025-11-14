#include "bmpch.hpp"
#include "Application.hpp"

namespace BM
{
	Application::Application(const ApplicationContext& context) noexcept
		: m_Context(context), m_Window(context._WindowContext)
	{
		m_Window.Create();

		s_Instance = this;
	}

	Application::~Application() noexcept
	{
		s_Instance = nullptr;
	}

	Application& Application::Get() noexcept
	{
		assert(s_Instance != nullptr);

		return *s_Instance;
	}

	void Application::Run()
	{
		m_Running = true;

		while (m_Running)
		{
			while (auto event = m_Window.PollEvent())
			{
				if (m_Context._WindowDefaultEventHandler)
					m_Window.OnEvent(event.value());
			}

			if (!m_Window.IsOpen())
				Stop();

			m_Window.ClearScreen();
			m_Window.DisplayScreen();
		}
	}

	void Application::Stop()
	{
		m_Running = false;
	}
}