#pragma once
#include "Window.hpp"
#include "EventDispatcher.hpp"
#include "Layer/LayerMachine.hpp"
#include "Asset/AssetManager.hpp"
#include <concepts>
#include <memory>

namespace BM
{
	struct ApplicationContext
	{
		WindowContext Window;

		bool DefaultWindowCloseEvent = true;
	};

	class Application
	{
	public:
		Application(const ApplicationContext& context = {}) noexcept;
		~Application() noexcept;

		static Application& Get() noexcept;

		void Run();
		void Stop();

		Window& GetWindow() noexcept;
		Renderer& GetRenderer() noexcept;
		LayerMachine& GetMachine() noexcept;
		AssetManager& GetAssets() noexcept;

		template<std::derived_from<Layer> TLayer, typename... TArgs>
		inline void QueuePushLayer(TArgs&&... args) noexcept {
			BM_CORE_FN();
			m_Machine.QueuePush(std::move(std::make_unique<TLayer>(std::forward<TArgs>(args)...)));
		}
	private:
		void EventCallback(Event& event) noexcept;

		bool OnCloseEvent(const EventHandle::Closed& event) noexcept;
		bool OnResizeEvent(const EventHandle::Resized& event) noexcept;
	private:
		ApplicationContext m_Context;
		std::unique_ptr<Window> m_Window;
		LayerMachine m_Machine;
		AssetManager m_Assets;

		bool m_Running = false;
	private:
		static inline Application* s_Instance = nullptr;
	};
}
