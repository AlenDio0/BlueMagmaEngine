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
		bool StopOnWindowCloseEvent = true;

		uint32_t TPSLimit = 30u;
		float MaxLagTime = 1.f;
		float TimeScale = 1.f;
	};

	//======================================================================================

	class Application
	{
	public:
		LayerMachine Layers;
		AssetManager Assets;
	public:
		Application(const ApplicationContext& appContext = {}) noexcept;
		~Application() noexcept;

		static Application& Get() noexcept;

		//======================================================================================

		void SetDefaultWindowCloseEvent(bool flag) noexcept;
		void SetStopOnWindowCloseEvent(bool flag) noexcept;
		void SetTPSLimit(uint32_t tps) noexcept;
		void SetMaxLagTime(float lag) noexcept;
		void SetTimeScale(float timeScale) noexcept;

		const ApplicationContext& GetContext() const noexcept;

		//======================================================================================

		void Run();
		void Stop();

		//======================================================================================

		void CreateOrReplaceWindow(WindowContext windowContext = {}) noexcept;

		Window& GetWindow() noexcept;
		Renderer& GetRenderer() noexcept;

		//======================================================================================

		template<std::derived_from<Layer> TLayer, typename... TArgs>
		inline void QueuePushLayer(TArgs&&... args) noexcept {
			BM_CORE_FN();
			Layers.QueuePush(std::move(std::make_unique<TLayer>(std::forward<TArgs>(args)...)));
		}
	private:
		void EventCallback(Event& event) noexcept;

		bool OnCloseEvent(const EventHandle::Closed& event) noexcept;
	private:
		ApplicationContext m_Context;

		std::unique_ptr<Window> m_Window;

		bool m_Running = false;
	private:
		static inline Application* s_Instance = nullptr;
	};
}
