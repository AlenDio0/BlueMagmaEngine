#include "bmpch.hpp"
#include "Application.hpp"
#include <SFML/System/Clock.hpp>

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

		sf::Clock deltaClock;
		while (m_Running)
		{
			float deltaTime = deltaClock.restart().asSeconds();

			if (!m_Machine.ProcessLayerChanges())
			{
				Stop();
				break;
			}

			const auto& layers = m_Machine.GetLayers();

			while (auto event = m_Window.PollEvent())
			{
				if (m_Context._WindowDefaultEventHandler)
					m_Window.OnEvent(*event);

				for (const auto& layer : layers)
					layer->OnEvent(*event);
			}

			if (!m_Window.IsOpen())
				Stop();

			for (const auto& layer : layers)
				layer->OnUpdate(deltaTime);

			m_Window.ClearScreen();

			for (const auto& layer : layers)
				layer->OnRender(m_Window.GetHandle());

			m_Window.DisplayScreen();
		}
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	Window& Application::GetWindow() noexcept
	{
		return m_Window;
	}

	AssetManager& Application::GetAssets() noexcept
	{
		return m_Assets;
	}

	void Application::TransitionLayer(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept
	{
		m_Machine.TransitionLayer(fromLayer, std::move(toLayer));
	}

	void Application::RemoveLayer(Layer* layer) noexcept
	{
		m_Machine.RemoveLayer(layer);
	}
}
