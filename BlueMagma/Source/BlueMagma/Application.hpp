#pragma once
#include <string>
#include <cstdint>
#include <SFML/Graphics/RenderWindow.hpp>

namespace BM
{
	struct ApplicationContext
	{
		std::string _WindowTitle = "BlueMagma Application";
		uint32_t _WindowWidth = 800, _WindowHeight = 800;
	};

	class Application
	{
	public:
		Application(const ApplicationContext& context = {}) noexcept;
		~Application() noexcept;

		static Application& Get() noexcept;

		void Run();
		void Stop();
	private:
		ApplicationContext m_Context;
		sf::RenderWindow m_Window;

		bool m_Running = false;
	private:
		static inline Application* s_Instance = nullptr;
	};
}
