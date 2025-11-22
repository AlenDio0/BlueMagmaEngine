#pragma once
#include "Window.hpp"
#include "LayerMachine.hpp"
#include "AssetManager.hpp"
#include "Asset.hpp"
#include <memory>
#include <concepts>
#include <string>

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

		Window& GetWindow() noexcept;
		AssetManager& GetAssets() noexcept;

		template<std::derived_from<Layer> TLayer>
		inline TLayer* GetLayer() const noexcept {
			return m_Machine.GetLayer<TLayer>();
		}
		template<std::derived_from<Layer> T, typename... Args>
		inline void PushLayer(Args&&... args) noexcept {
			m_Machine.PushLayer(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
		}
		void TransitionLayer(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept;
		void RemoveLayer(Layer* layer) noexcept;
	private:
		ApplicationContext m_Context;
		Window m_Window;
		LayerMachine m_Machine;
		AssetManager m_Assets;

		bool m_Running = false;
	private:
		static inline Application* s_Instance = nullptr;
	};
}
