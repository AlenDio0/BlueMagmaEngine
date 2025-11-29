#pragma once
#include "EventDispatcher.hpp"
#include "Window.hpp"
#include "AssetManager.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
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
		inline virtual void OnRender(sf::RenderTarget& target) noexcept {}
	protected:
		Application& GetApp() const noexcept;

		Window& GetWindow() const noexcept;
		LayerMachine& GetMachine() const noexcept;
		AssetManager& GetAssets() const noexcept;

		template<std::derived_from<Layer> TLayer, typename... Args>
		inline void QueueTransitionTo(Args&&... args) noexcept {
			BM_CORE_FN();
			QueueTransition(std::move(std::make_unique<TLayer>(std::forward<Args>(args)...)));
		}
		void QueueRemoveLayer() noexcept;

		template<std::derived_from<AssetHandle> TAsset>
		inline const TAsset* GetAsset(const std::string& key) const noexcept {
			return GetAssets().Get<TAsset>(key);
		}
	private:
		void QueueTransition(std::unique_ptr<Layer> toLayer) noexcept;
	};
}
