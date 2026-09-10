#pragma once
#include "Layer/LayerMachine.hpp"

#include "Event/Event.hpp"
#include "Graphics/Window.hpp"
#include "Asset/AssetManager.hpp"

#include <concepts>
#include <memory>
#include <cstdint>

namespace BM
{
	struct ApplicationContext
	{
		uint32_t TPSLimit = 30u;
		float MaxLagTime = 1.f;
		float TimeScale = 1.f;

		bool DefaultWindowCloseEvent = true;
		bool StopOnWindowCloseEvent = true;
	};

	//======================================================================================

	class AppLayer;

	//======================================================================================

	class Application
	{
	public:
		LayerMachine Layers;
		AssetManager Assets;
	public:
		Application(const ApplicationContext& appContext = {}) noexcept;
		virtual ~Application() noexcept;

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

		std::weak_ptr<Window> GetWindow() noexcept;
		std::weak_ptr<Renderer> GetRenderer() noexcept;

		//======================================================================================

		template<std::derived_from<AppLayer> TAppLayer, typename... TArgs>
		inline void QueuePushLayer(TArgs&&... args) noexcept {
			BM_CORE_DEBUG_FN("Requested to push an AppLayer into the LayerMachine");

			auto layer = std::make_unique<TAppLayer>(std::forward<TArgs>(args)...);
			layer->AttachApplication(this);
			Layers.QueuePush(std::move(layer));
		}
	private:
		void EventCallback(Event& event) noexcept;

		bool OnCloseEvent(const EventHandle::Closed& event) noexcept;
	private:
		ApplicationContext m_Context;

		std::shared_ptr<Window> m_Window;

		bool m_Running = false;
	};
}
