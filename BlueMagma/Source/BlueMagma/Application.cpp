#include "Application.hpp"
#include <SFML/Window/VideoMode.hpp>
#include <cassert>

namespace BM
{
	Application::Application(const ApplicationContext& context) noexcept
		: m_Context(context), m_Window(sf::VideoMode({ context._WindowWidth, context._WindowHeight }), context._WindowTitle)
	{
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

		}
	}

	void Application::Stop()
	{
		m_Running = false;
	}
}