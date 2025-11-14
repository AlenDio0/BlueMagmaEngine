#pragma once
#include "Window.hpp"

namespace BM
{
	struct ApplicationContext
	{
		WindowContext _WindowContext;

		bool _WindowDefaultEventHandler = true;
	};

	class Application
	{
	public:
		Application(const ApplicationContext& context = {}) noexcept;
		~Application() noexcept;

		static Application& Get() noexcept;

		void Run();
		void Stop();

		const Window& GetWindow() const noexcept {
			return m_Window;
		}
	private:
		ApplicationContext m_Context;
		Window m_Window;

		bool m_Running = false;
	private:
		static inline Application* s_Instance = nullptr;
	};
}
