#pragma once
#include "Layer.hpp"
#include <vector>
#include <memory>
#include <utility>

namespace BM
{
	class LayerMachine
	{
	public:
		void PushLayer(std::unique_ptr<Layer> layer) noexcept;
		void TransitionLayer(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept;
		void RemoveLayer(Layer* layer) noexcept;

		template<std::derived_from<Layer> TLayer>
		inline TLayer* GetLayer() const noexcept {
			for (const auto& layer : m_Layers)
			{
				if (TLayer* tLayer = dynamic_cast<TLayer*>(layer.get()))
					return tLayer;
			}
			return nullptr;
		}
		const std::vector<std::unique_ptr<Layer>>& GetLayers() noexcept;

		bool ProcessLayerChanges() noexcept;
	private:
		void OnRemove() noexcept;
		void OnTransition() noexcept;
	private:
		std::vector<std::unique_ptr<Layer>> m_Layers;

		Layer* m_RemoveLayer = nullptr;
		struct Transition
		{
			Layer* _FromLayer = nullptr;
			std::unique_ptr<Layer> _ToLayer;

			operator bool() const noexcept {
				return _FromLayer && _ToLayer;
			}
		} m_Transition;
	};
}
