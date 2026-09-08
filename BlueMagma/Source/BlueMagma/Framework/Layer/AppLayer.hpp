#pragma once
#include "Layer.hpp"
#include "LayerMachine.hpp"

#include "Framework/Application.hpp"

#include <concepts>
#include <memory>

namespace BM
{
	class AppLayer : public Layer
	{
	public:
		inline AppLayer() noexcept = default;
		inline virtual ~AppLayer() noexcept override = default;

		AppLayer(const AppLayer&) = delete;
		AppLayer(AppLayer&&) = delete;
		AppLayer& operator=(const AppLayer&) = delete;
		AppLayer& operator=(AppLayer&&) = delete;

		inline virtual void OnAttachApplication() noexcept {}

		void AttachApplication(Application* applicationPtr) noexcept;
	protected:
		Application& GetApp() const noexcept;

		Window& GetWindow() const noexcept;
		Renderer& GetRenderer() const noexcept;
		LayerMachine& GetLayers() const noexcept;
		AssetManager& GetAssets() const noexcept;

		template<std::derived_from<AppLayer> TAppLayer, typename... TArgs>
		inline void QueueTransitionTo(TArgs&&... args) noexcept {
			BM_CORE_DEBUG_FN("Requested from AppLayer to transition to another AppLayer");

			auto layer = std::make_unique<TAppLayer>(std::forward<TArgs>(args)...);
			layer->AttachApplication(m_ApplicationPtr);
			QueueTransition(std::move(layer));
		}
		void QueueRemoveLayer() noexcept;

		template<std::derived_from<AssetHandle> TAsset>
		inline const TAsset& GetAsset(const std::string& key) const noexcept {
			return GetAssets().Get<TAsset>(key);
		}
	private:
		void QueueTransition(std::unique_ptr<Layer> toLayer) noexcept;
	private:
		Application* m_ApplicationPtr = nullptr;
	};
}
