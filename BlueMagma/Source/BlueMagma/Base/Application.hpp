#pragma once
#include "Window.hpp"
#include "EventDispatcher.hpp"
#include "Layer/LayerMachine.hpp"
#include "Asset/AssetManager.hpp"
#include <concepts>
#include <memory>
#include <cstdint>

namespace BM
{
	struct ApplicationContext
	{
		bool DefaultWindowCloseEvent = true;

		uint32_t TPSLimit = 30u;
		float MaxLagTime = 1.f;
		float TimeScale = 1.f;
	};

	class Application
	{
	public:
		ApplicationContext Context;
	public:
		Application(const ApplicationContext& appContext = {}, WindowContext windowContext = {}) noexcept;
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
	private:
		std::unique_ptr<Window> m_Window;
		LayerMachine m_Machine;
		AssetManager m_Assets;

		bool m_Running = false;
	private:
		static inline Application* s_Instance = nullptr;
	};
}
