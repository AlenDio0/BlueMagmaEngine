#pragma once
#include "Layer.hpp"
#include <vector>
#include <memory>
#include <concepts>
#include <variant>

namespace BM
{
	class LayerMachine
	{
	public:
		void RemoveLayer(Layer* layer) noexcept;
		void TransitionLayer(Layer* fromLayer, std::unique_ptr<Layer> toLayer) noexcept;
		void PushLayer(std::unique_ptr<Layer> layer) noexcept;

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
		struct RemoveOperation
		{
			Layer* _Layer;
		};
		struct TransitionOperation
		{
			Layer* _FromLayer = nullptr;
			std::unique_ptr<Layer> _ToLayer;
		};
		struct PushOperation
		{
			std::unique_ptr<Layer> _Layer;
		};
	private:
		void HandleOperation(const RemoveOperation& remove) noexcept;
		void HandleOperation(TransitionOperation& transition) noexcept;
		void HandleOperation(PushOperation& push) noexcept;
	private:
		std::vector<std::unique_ptr<Layer>> m_Layers;

		using Operation = std::variant<RemoveOperation, TransitionOperation, PushOperation>;
		std::vector<Operation> m_OperationBuffer;
	};
}
