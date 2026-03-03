#pragma once
#include "Layer.hpp"
#include "Base/Application.hpp"
#include "Base/Window.hpp"
#include "Base/Renderer.hpp"
#include "LayerMachine.hpp"
#include "Asset/AssetManager.hpp"
#include <concepts>
#include <memory>

namespace BM
{
	class AppLayer : public Layer
	{
	public:
		inline AppLayer() noexcept = default;
		inline virtual ~AppLayer() noexcept = default;
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
