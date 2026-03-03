#pragma once
#include "Base/Window.hpp"
#include "Base/Renderer.hpp"
#include "Base/EventDispatcher.hpp"
#include "Asset/AssetManager.hpp"
#include <concepts>
#include <memory>

namespace BM
{
	class Application;
	class LayerMachine;

	class Layer
	{
	public:
		inline Layer() noexcept = default;
		inline virtual ~Layer() noexcept = default;

		inline virtual void OnAttach() noexcept {}
		inline virtual void OnDetach() noexcept {}
		inline virtual void OnTransition() noexcept {}

		inline virtual void OnEvent(Event& event) noexcept {}
		inline virtual void OnUpdate(float deltaTime) noexcept {}
		inline virtual void OnRender() noexcept {}
	protected:
		Application& GetApp() const noexcept;

		Window& GetWindow() const noexcept;
		Renderer& GetRenderer() const noexcept;
		LayerMachine& GetMachine() const noexcept;
		AssetManager& GetAssets() const noexcept;

		template<std::derived_from<Layer> TLayer, typename... TArgs>
		inline void QueueTransitionTo(TArgs&&... args) noexcept {
			BM_CORE_FN();
			QueueTransition(std::move(std::make_unique<TLayer>(std::forward<TArgs>(args)...)));
		}
		void QueueRemoveLayer() noexcept;

		template<std::derived_from<AssetHandle> TAsset>
		inline const TAsset& GetAsset(const std::string& key) const noexcept {
			return GetAssets().Get<TAsset>(key);
		}
	private:
		void QueueTransition(std::unique_ptr<Layer> toLayer) noexcept;
	};
}
